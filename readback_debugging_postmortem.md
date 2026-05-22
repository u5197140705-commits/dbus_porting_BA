# Readback Bring-Up Postmortem

Date: 2026-05-22
Scope: DBUS readback stabilization across board variants

## Executive Summary

The readback issue was difficult because it was not a single bug. It was a system-level interaction problem across protocol framing, SPI timing, board configuration, initialization order, and observability. Each flashing cycle usually validated only one hypothesis, so progress happened in layers rather than in one final fix.

In short, we were debugging an emergent behavior on real hardware, not a deterministic compile-time error.

## Why This Took Many Iterations

### 1. Readback Is Timing-Critical and State-Dependent

Readback depends on exact timing boundaries:
- chip-select assertion and release timing
- command/address phase boundaries
- dummy-byte placement and count
- when the slave starts driving valid MISO data
- when the master samples returned data

Small timing mismatches can produce symptoms that look random (all-zero bytes, shifted bytes, stale bytes, or intermittent success). Code review alone cannot prove correctness here; hardware validation is required.

### 2. Write Path Success Did Not Guarantee Read Path Correctness

Write often looked stable earlier, which created a false sense of protocol health. But readback can require pipeline-aware handling (for example, request in one frame and valid data in a subsequent phase/frame). That made off-by-one-frame and off-by-one-byte defects likely.

### 3. Full-Duplex SPI Semantics Masked Root Causes

In full duplex:
- transmitted control bytes and received payload bytes coexist on the same clock stream
- buffer indexing errors are easy to hide
- stale data can look like valid data if alignment is accidentally consistent

This made logs appear plausible even when phase alignment was wrong.

### 4. Multi-Variant Board Porting Increased the Search Space

Every failure could come from either:
- common protocol logic, or
- board-specific details (pinmux, clock tree, SPI mode, frequency ceilings, interrupt behavior, DMA policy)

Because both categories were active during bring-up, each test had compounded uncertainty.

### 5. Flash/Reset Cycles Changed Runtime Conditions

Some defects were sequence-sensitive and only reproducible after specific startup paths:
- cold boot vs warm reset differences
- initial peripheral state after flashing
- startup race conditions between tasks/ISRs and protocol service initialization

A patch that worked in one runtime context could fail after reflashing, forcing repeated validation passes.

### 6. Configuration Coupling Across Layers

Readback behavior depended on agreement between multiple layers:
- protocol constants and framing assumptions
- driver transaction boundaries
- ISR/DMA buffer ownership and lifetime
- build-time configuration and feature flags

One wrong macro, stale generated setting, or mismatched constant could invalidate an otherwise correct code change.

### 7. Limited Early Observability Slowed Convergence

Early debugging lacked complete instrumentation at key boundaries:
- exact tx/rx windows per transfer
- frame/phase annotations
- CS edge timing markers
- buffer snapshots before and after ISR/DMA completion

Without this, each iteration had high ambiguity, so many flashes were spent improving diagnosis before the root issue became obvious.

### 8. Multiple "Almost Correct" Intermediate States

The system passed through several plausible but incorrect states:
- command parsed, wrong register/offset
- offset correct, byte order wrong
- byte order correct, sampled one cycle late
- first transaction invalid, subsequent transactions valid

Each partial state changed the symptom pattern, which felt like moving targets even while converging.

## Why Reflashing Was Necessary (Not Wasteful)

Reflashing was required because many hypotheses were only testable on-device:
- protocol and timing defects are hardware-realized
- reset-state dependence must be observed after actual firmware image load
- peripheral driver behavior (interrupt latencies, DMA completion ordering, FIFO effects) is platform-specific

For this issue class, iteration count is expected to be high unless instrumentation is added early and test order is tightly controlled.

## Root Difficulty Pattern

This can be summarized as:

1. High coupling across layers
2. Hardware-timing sensitivity
3. Incomplete observability at start
4. Variant-specific uncertainty

When these four combine, debugging becomes iterative by nature.

## What We Learned

### Technical Learnings

- Readback correctness must be validated with explicit frame/phase accounting, not inferred from successful writes.
- Protocol handling should encode the expected pipeline behavior (when data becomes valid) as a first-class contract.
- SPI timing assumptions should be documented and checked per target variant.
- Bring-up should separate core protocol verification from board-specific tuning to reduce branching uncertainty.

### Process Learnings

- Instrumentation-first debugging would have reduced the number of blind flashes.
- A fixed test matrix (cold boot, warm reset, repeated reads, boundary registers) would have made regressions easier to classify.
- Capturing "known good" traces early would have provided a concrete comparison target.

## Recommended Debug Playbook for Next Time

### Phase 1: Lock Protocol Contract

- Define exact frame structure and readback validity point.
- Document dummy bytes, endian rules, and response alignment.
- Add assertions for frame length and phase transitions where possible.

### Phase 2: Add Observability Before Deep Changes

- Log tx/rx bytes per transfer with frame labels.
- Mark CS edges and transfer indices.
- Snapshot buffers before submit and after completion callbacks.
- Add per-transaction identifiers to correlate command and response.

### Phase 3: Run Deterministic Test Matrix

- Cold boot single read
- Warm reset single read
- Burst repeated reads of same register
- Sequential reads across adjacent registers
- Boundary/edge register reads

Record pass/fail signatures for each to quickly classify bug type.

### Phase 4: Split Common vs Variant Validation

- Validate protocol on one reference board first.
- Freeze protocol code once stable.
- Then tune per-board SPI/clock/pinmux settings one variant at a time.

### Phase 5: Reduce Flash Cost

- Keep one minimal readback diagnostic build profile.
- Minimize unrelated code movement between tests.
- Preserve a known-good baseline binary for A/B comparison.

## Fast Triage Heuristics

Use these heuristics to narrow root cause quickly:

- All zeros consistently: likely slave-drive timing, CS window, or wrong phase.
- Correct bytes but shifted: likely pipeline/dummy-byte alignment error.
- Correct after first transfer only: likely initialization/flush state issue.
- Variant-only failure: likely board SPI mode/clock/pinmux or IRQ/DMA differences.
- Intermittent failures at higher speed: likely timing margin or signal integrity.

## Exit Criteria for "Readback Stable"

Readback should be considered stable only when:
- deterministic pass over the defined matrix
- repeatability across cold/warm starts
- repeatability across selected board variants
- no dependency on debug logging side effects
- documented protocol contract and board-specific deviations

## Chronological Iteration Timeline (What Each Flash Taught Us)

This timeline is derived from the project session logs and highlights why each step required real device reflash/retest cycles.

### 2026-03-18: Frame alignment and serial-shift compensation phase

Primary symptom:
- PRE_MOTOR checks failing with shifted or zero readback values.

Key hypotheses tested:
- CS/frame boundary handling on the Pico slave was misaligned.
- Link-level MISO transform required explicit compensation.

Changes introduced:
- Reworked Pico frame servicing to synchronize on CS boundaries.
- Adjusted transmit/refill handling to avoid stale or shifted response windows.
- Added marker-tolerant response matching and serial-shift compensation in RW612/Pico path.

What repeated flashing validated:
- Whether readback improved from offset/zero patterns to stable register matches.
- Whether improvements held across repeated PRE_MOTOR sequences after reboot.

Why this still took multiple passes:
- Small changes in CS boundary behavior can shift all downstream bytes.
- The same visible error pattern could originate from different sub-causes.

### 2026-04-02: False-negative software state phase

Primary symptom:
- Contradictory runtime logs showed SPI init/send failures while lower-level activity still looked operational.

Key hypotheses tested:
- Error handling was reporting failures incorrectly due to return-value semantics.

Changes introduced:
- Fixed success checks for SPI init/send paths (`0` success semantics).
- Corrected return flow to match real operation result.

What repeated flashing validated:
- That false error logs disappeared on hardware boots.
- That actual transport behavior could now be interpreted without misleading software status.

Why this still required board cycles:
- Logging correctness had to be verified during true startup/runtime conditions, not only in static code inspection.

### 2026-04-07: Physical CS-path isolation phase

Primary symptom:
- RW612 showed active SCK edges, but Pico counters indicated no CS transitions and no RX bytes.

Key hypotheses tested:
- Pico slave peripheral may have been misconfigured (PL022 sequencing issue).
- CS routing/wiring/pin-selection could be wrong even if clock was present.

Changes introduced:
- Corrected Pico PL022 slave-mode sequencing (SSE handling around slave config).
- Reordered GPIO function assignment and added stronger diagnostics/heartbeat register visibility.

What repeated flashing validated:
- Peripheral configuration became correct, but CS activity was still absent in failing runs.
- The problem domain narrowed from protocol logic to physical/CS path integrity.

Why this took iterations:
- Electrical routing and pinmux mismatches only become obvious under live traffic.
- A correct SPI block can still fail completely if CS is not the expected line.

### 2026-04-08 to 2026-04-10: Stability and jitter-reduction phase

Primary symptom:
- Communication partially worked but remained intermittent with occasional dummy/zero artifacts.

Key hypotheses tested:
- FIFO servicing and CPU jitter on Pico hot path were creating timing slips.
- SPI rate and inter-frame idle gaps affected readback stability margins.

Changes introduced:
- Reduced/tuned SPI speed for stress and stability windows.
- Prioritized receive servicing and reduced blocking/non-deterministic operations in hot path.
- Added inter-frame idle handling and adaptive CS-end drain behavior.

What repeated flashing validated:
- Whether pass rate improved toward deterministic multi-run success.
- Whether transient artifacts reduced under repeated long-run tests.

Why this took many flashes:
- Jitter/timing improvements are statistical; one run is not enough.
- Stability requires repeated evidence over many cycles, not a single green run.

### 2026-05-08 to 2026-05-14: Functional fallback and scope-control phase

Primary symptom:
- Motors controlled reliably on write path, but native readback remained malformed/intermittent.

Key hypotheses tested:
- Immediate delivery goals could proceed with fallback/shadow behavior while preserving future readback recovery path.

Changes introduced:
- Enabled readback bypass/fallback paths for continuity of motor-control validation.
- Temporarily disabled strict readback verification gates to continue functional system tests.

What repeated flashing validated:
- End-to-end motor control remained stable across multiple devices.
- Failures were isolated specifically to native read-response robustness, not control command delivery.

Why this was still iterative:
- Needed to separate "system can operate" from "diagnostics/readback fully solved."
- Each flash confirmed that scope reduction did not hide a control-path regression.

### 2026-05-19: Deep readback root-cause isolation phase

Primary symptom:
- Persistent dummy read response pattern (`40 00 00 00 00 00 00 00`) despite apparent Pico TX preload activity.

Key hypotheses tested:
- RW612 bytewise transfer path was not truly bytewise in behavior.
- Pico was rearming/resetting SPI in unsafe windows (mid-transfer).
- Rearm implementation (`spi_deinit`/`spi_init`) introduced side effects (transient master-mode behavior, spurious CS-end logic).

Changes introduced:
- Made RW612 transceive truly bytewise under one CS assertion.
- Removed unsafe rearm/queue operations from sniff/streaming paths.
- Moved toward CS-end-only safe rearm model.
- Replaced heavy reinit-style rearm with safer SSE-toggle approach.

What repeated flashing validated:
- Which changes altered first-byte behavior versus full-frame behavior.
- Whether partial progress (known first-byte pattern) was real or incidental.
- Which candidate paths still triggered frame disruption.

Why this phase required dense iteration:
- Multiple independent defects interacted in the same symptom signature.
- Fixing one source of corruption exposed the next bottleneck, requiring new flash/retest loops.

## Timeline-Level Conclusion

The long iteration count came from progressive uncertainty reduction:
- first remove false software signals
- then isolate physical signaling correctness
- then stabilize timing/jitter behavior
- then separate functional delivery from diagnostic purity
- finally chase remaining readback corner cases at frame/sub-frame level

In other words, each flash was not repeating the same test; it was retiring a different class of risk.

## Flash Iteration Decision Tree (Quick Use)

Use this sequence after each flash to choose the next highest-value experiment.

### Step 0: Pre-Flight (Before Running Test)

- Confirm both binaries are current and from intended commit/build output.
- Confirm board power/reset state and cabling are unchanged from prior run.
- Confirm logging level includes tx/rx frame bytes and CS-edge counters.

If any item fails, fix pre-flight first and reflash only once.

### Step 1: Classify Immediate Symptom

Pick one dominant symptom from the first failing run:

- A) No CS activity on slave, but SCK edges present
- B) CS and clocks present, RX mostly zeros
- C) RX first byte looks plausible, remaining bytes zero/garbled
- D) Correct data appears shifted/off-by-one
- E) First transaction fails, later transactions pass
- F) Intermittent failures only after several cycles or at higher speed

### Step 2: Branch Actions

#### Branch A: No CS activity on slave

- Validate CS pin routing/pinmux on both sides.
- Compare against last known-good CS pin map.
- Add temporary multi-pin edge scan if uncertain.

Next flash objective:
- Prove CS transitions are observed on expected slave pin.

Stop branch when:
- CS counters increment reliably for every master transaction.

#### Branch B: Clocks and CS present, RX mostly zeros

- Validate slave-drive timing and response-valid phase.
- Validate dummy-byte count and read window placement.
- Check rearm/reset paths are not running during active transfer.

Next flash objective:
- Observe non-zero pattern aligned with expected response phase.

Stop branch when:
- Non-zero response appears deterministically in expected window.

#### Branch C: First byte plausible, rest zero/garbled

- Inspect FIFO load completeness and tx index progression.
- Confirm no mid-transfer peripheral reset/reinit path exists.
- Confirm response queue/load happens only in CS-safe point.

Next flash objective:
- Achieve full 8-byte coherent response, not only first-byte signature.

Stop branch when:
- Full-frame bytes are stable across 10 consecutive reads.

#### Branch D: Data shifted/off-by-one

- Verify frame boundary reset (CS-end handling).
- Verify pipeline model (request frame vs response frame timing).
- Re-check bit/byte transform assumptions on wire.

Next flash objective:
- Eliminate consistent offset pattern across repeated reads.

Stop branch when:
- Expected register bytes align without offset in repeated runs.

#### Branch E: First transaction fails, later pass

- Compare cold boot vs warm reset behavior.
- Inspect initialization ordering and first-frame preloading.
- Add explicit first-transaction guard/flush if protocol permits.

Next flash objective:
- Make first post-boot read equal in quality to steady-state reads.

Stop branch when:
- Cold and warm start first-read behavior matches steady state.

#### Branch F: Intermittent/stress-only failures

- Reduce speed and remove non-deterministic hot-path work.
- Add inter-frame idle margin and adaptive CS-end drain.
- Run deterministic repeatability matrix (20+ runs).

Next flash objective:
- Convert sporadic pass into sustained repeatability.

Stop branch when:
- Defined repeatability threshold (for example, 20/20) is met.

### Step 3: Decide Next Flash Type

Choose exactly one change category per flash cycle:

- Protocol framing change
- SPI timing/speed/idle change
- Peripheral rearm/reset behavior change
- Diagnostics-only instrumentation change
- Physical routing/pinmux correction

Rule:
- Never combine multiple behavior-changing categories in one flash.

### Step 4: Evidence Checklist Per Run

Capture these artifacts every cycle:

- RW612 tx/rx frame log for failing and passing examples
- Pico CS edge counts and FIFO/state counters
- Boot context (cold vs warm)
- Exact firmware identifiers flashed to each board

If evidence is incomplete, do not branch conclusions yet.

### Step 5: Exit Conditions

Declare branch resolved only when both are true:

- Symptom class no longer reproduces in the matrix.
- Neighboring behavior did not regress (write path, motor control, startup health).

If resolved, return to Step 1 and classify the next-most-severe remaining symptom.

## Closing Note

The large number of flashes was a normal consequence of hardware-in-the-loop debugging under high coupling and limited initial visibility. The work still converged correctly; the cost came from discovering and eliminating one uncertainty class at a time.
