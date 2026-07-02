# Session Protocol — 2026-07-02

## Objective
Preserve the known-good all-4-motor motion baseline and debug the isolated
RW612 secondary-Pico readback path until `DBCDRV_readReg32()` returns real,
repeatable register values instead of timeouts, provisional matches, or
header-contaminated first reads.

---

## Guardrails
- Do not touch the stable all-4-motor motion baseline.
- Keep experimental RW612 work isolated in the detached tree:
  `dbus_porting_BA_known_good_2fd321d/zephyr_dbus_driver`
- Use distinct artifact names for every Pico UF2 and every major RW612 probe.

---

## Starting State
- Stable motion already existed and had to remain untouched.
- Active issue was narrowed to Pico2-only readback on the RW612 secondary SPI path.
- Earlier sessions had already shown that write-driven control worked while readback
  was timing/framing-sensitive.
- RW612 strict diagnostic logs were the main discriminator for every iteration.

---

## Main Iterations This Session

### 1. Pico exact 8-byte frame ownership tightened
Changes on Pico2 firmware in `pico_spi_slave_test/main.c`:
- short sniff captures (`sniff_len <= 8`) made diagnostic-only
- stale `rx_index` dropped on `cs_start`
- exact 8-byte frames validated with exact command and `len_words == 1`

Reason:
- avoid duplicate ownership between sniff path and authoritative cs_end RX-FIFO path
- reject false-positive transformed decodes committing bad motor values

Result:
- broad corruption reduced
- remaining failures became coherent full reply frames with wrong or stale payloads

Key artifacts:
- `helper/pico_spi_slave_test_pico2_nodup_flushrearm_2026-07-02.uf2`
- `helper/pico_spi_slave_test_pico2_nodup_flushrearm_nosniffshort_2026-07-02.uf2`
- `helper/pico_spi_slave_test_pico2_nodup_flushrearm_nosniffshort_rxguard_2026-07-02.uf2`

---

### 2. RW612 newest-first read matching tested and falsified
Changes on RW612 in `dbus_driver.c`:
- reversed cumulative response scan preference so newest valid response would win

Reason:
- some failures looked like RW612 selecting an older valid fragment

Result:
- falsified by strict logs
- wrong full lane-0 response frames were already visible before RW612 matching

Conclusion:
- root cause was not just stale fragment selection on RW612

---

### 3. Pico identity-only exact 8-byte write decode tested and falsified
Change on Pico2:
- constrained exact 8-byte RW612 register frame decode to `TRANSFORM_IDENTITY` only

Reason:
- hypothesis: transformed MOSI decodes were falsely validating headers and
  committing wrong motor-speed payloads

Artifact:
- `helper/pico_spi_slave_test_pico2_identity8_rxguard_2026-07-02.uf2`

Result:
- falsified
- strict logs still showed failures, but now the transport symptom became clearer

---

### 4. Pico cs_start rearm / TX restart probes
Changes on Pico2:
- reset `tx_index = 0` whenever cs_start rearmed the SPI block
- later forced every pending read response on Pico2 to rearm and restart from byte 0

Reason:
- lane-0 logs showed circularly shifted reply forms such as:
  - `50 14 01 00 00 00 00 a0`
  - `00 a0 50 34 01 00 00 00`
  - `50 34 01 f4 01 00 00 a0`

Artifacts:
- `helper/pico_spi_slave_test_pico2_rearm_txrestart_2026-07-02.uf2`
- `helper/pico_spi_slave_test_pico2_readrestart_eachcs_2026-07-02.uf2`

Result:
- not sufficient by itself
- stale/rotated reply behavior reduced the search space but did not fully solve readback

Conclusion:
- cs boundary handling was part of the story, but not the final controlling defect

---

### 5. RW612 logging shortened and active-tree mismatch discovered
RW612 changes:
- reduced readback logging to keep one-line summaries and suppress large lane/tail dumps
- later added a compact lane-0 fingerprint line:
  `DBCDRV_readReg32 lane0 raw=... dec=...`

Important discovery:
- the first shorter-log edit was applied to the workspace copy of
  `zephyr_dbus_driver`, but the bench image was actually coming from the detached tree
  `dbus_porting_BA_known_good_2fd321d/zephyr_dbus_driver`

Result:
- once patched in the detached tree, logs became short enough to reason from quickly

Markers used:
- `PICO2_ALIAS_PROBE_V1_SHORTLOG_2026_07_02`

---

### 6. Compact lane-0 logs revealed the decisive parser direction
Compact lane-0 fingerprint showed that Pico was already producing full reply frames,
for example:
- `a0 50 34 01 4c 04 00 00`
- `a0 50 14 01 f4 01 00 00`

But RW612 still rejected them for motor-speed reads and only kept provisional
interleaved candidates.

Conclusion:
- Pico was producing real reply frames on a byte lane
- RW612 was too strict about which extracted forms counted as authoritative

---

### 7. RW612 byte-lane exact-frame acceptance enabled
Change on RW612 detached tree:
- promoted exact or circularly rotated full-frame byte-lane matches to authoritative
  before provisional interleaved fallback

Marker:
- `PICO2_ALIAS_PROBE_V1_LANEEXACT_2026_07_02`

Result:
- deadlock broke: RW612 started returning successful reads
- but first reads were often false positives or header-contaminated, for example:
  - `0xffffffff`
  - `0xf4011450`
  - `0x4c011450`
  - `0xb4011450`
- second reads were often correct, showing the parser was now accepting too much,
  too early

Conclusion:
- the exact-lane direction was correct
- the early acceptance rule was too loose

---

### 8. RW612 stricter lane-exact parser prepared
Latest detached-tree change:
- removed the markerless early accept path from `dbus_drv_extract_read_response()`
- removed rotated-window promotion from the early exact-byte-lane helper
- kept lane-exact recovery, but only for marker-aligned windows

New marker:
- `PICO2_ALIAS_PROBE_V1_LANEEXACT_STRICT_2026_07_02`

Status at end of session:
- built successfully
- staged into the normal flash path
- pending bench validation

---

### 9. Exact reconstructed Pico rollbacks were not equivalent to saved good artifacts
Late-session bench work showed that rebuilding current `main.c` after removing only the
most recent experiment did not actually restore the last read-working behavior.

Observed result:
- multiple reconstructed rollback UF2s regressed to total read-response failure
- RW612 compact lane-0 tails became all `ff ff ff ff ff ff ff ff`
- every `DBCDRV_readReg32()` ended in timeout/error

Conclusion:
- source-level rollback by removing only the latest diff was not enough
- later Pico experiments still present in `main.c` changed behavior relative to the
  exact saved UF2s that had already been bench-validated
- from this point on, saved UF2 artifacts are the source of truth for behavior recovery

---

### 10. Saved Pico UF2 restored the useful late-session failure shape
Flashing the saved artifact:
- `helper/pico_spi_slave_test_pico2_nodup_flushrearm_nosniffshort_rxguard_2026-07-02.uf2`

with RW612 marker:
- `PICO2_ALIAS_PROBE_V1_LANEEXACT_NEWEST_2026_07_02`

restored live reads and narrowed the remaining bug to a Pico-side write-commit issue.

Recovered behavior shape:
- priming: motor1 (`0x5014`) read back `0x00000000`, motor3 (`0x5034`) read back correct `0x0000044C`
- step 1: motor1 still stuck at `0x00000000`, motor3 correct at `0x0000044C`
- step 2: both motors correct (`0x0000044C`, `0x000001F4`)
- step 3: both reads cleanly replayed the full step-2 values instead of the new step-3 writes
- step 4: both motors correct again (`0x000005A0`, `0x000000A5`)

Conclusion:
- RW612 parsing was no longer the dominant blocker on this path
- the remaining issue had collapsed to Pico-side write ownership / commit timing
- two distinct residual symptoms remained:
  - motor1 speed register `0x5014` sometimes never commits the first expected value and stays at zero
  - step-3 writes can be missed entirely, causing a clean replay of step-2 values on both motors

---

### 11. Exact-8 sniffer fallback restored Pico READ decode
Later Pico USB logs showed the PL022 RX path was not seeing complete command frames
(`f8=0`, `f8ok=0`, `f8w=0`, `f8r=0`, `lrrn=0`) while the GPIO sniffer captured
plausible RW612 8-byte commands.

Probe:
- `helper/pico_spi_slave_test_pico2_exact8sniff_2026-07-02.uf2`

Result:
- exact `sniff_len == 8` frames promoted through existing `process_rx_frame()`
- Pico counters rose (`f8ok`, `f8w`, `f8r`, `lrrn`)
- real `0xA0` responses were prepared again
- RW612 still failed because the response stream repeated byte 0 (`a0 a0 ...`)

Conclusion:
- RX command decode was repaired enough for bench progress
- the dominant blocker moved to Pico TX byte advancement across RW612 dummy CS pulses

---

### 12. TX progress preservation improved multi-attempt reconstruction
Probe:
- `helper/pico_spi_slave_test_pico2_txprogress_2026-07-02.uf2`

Change:
- pending read responses preserve `tx_index` in the TX queue helpers instead of
  restarting at byte 0 on every retry CS pulse

Result:
- Pico logs showed `txi` advancing through the response bytes
- RW612 reconstructed some reads again, including shadow-trace reads and second
  motor-speed reads
- remaining failure shape became stale or shifted response lifetime rather than
  byte-0-only TX

Conclusion:
- RW612's cumulative byte-lane reconstruction depends on preserving Pico TX progress
  across dummy retries
- any response-retirement fix must not reset `tx_index` on every CS pulse

---

### 13. Immediate response retirement falsified; delayed-retire probe staged
Probe falsified:
- `helper/pico_spi_slave_test_pico2_rsponce_2026-07-02.uf2`

Observation:
- clearing `tx_read_response_pending` immediately when `tx_index >= 8` was too early
- Pico logs moved toward `pending=0`, default `txd=ff...`, and RW612 saw many all-`ff`
  read attempts

Conclusion:
- response lifetime must be tied to a physical CS boundary after the full frame has
  been queued, not to queue-helper completion alone

New staged probe:
- `helper/pico_spi_slave_test_pico2_rspdelay_2026-07-02.uf2`

Implementation note:
- queue helpers now mark a pending response for retirement after the full frame is
  accepted
- the pending response is actually retired at the next CS-end boundary before the
  newly finished command is decoded
- USB/COM logging remains enabled

---

### 14. TRUEBYTE RW612 bench result: shadow dump is now a suspect perturbation
Bench marker:
- `PICO2_ALIAS_PROBE_V1_TRUEBYTE_2026_07_03`

Observed result:
- some first trace/motor reads still returned coherent stale or previous values
- many subsequent reads returned all-`ff` lane tails and timed out
- the pre-read eight-register shadow dump consumed many read transactions before
  the motor-speed reads
- shadow counters showed decoded/response activity even when the visible trace fields
  were stale or unavailable, for example `rsp=7 dec=12` after step 1

Conclusion:
- the full shadow dump is no longer a passive diagnostic in this fragile transport
  state; it can perturb or drain the response sequence before the actual motor reads

New staged RW612 discriminator:
- `helper/pico2_alias_probe_readfirst_v1_2026-07-03.elf`

Marker:
- `PICO2_ALIAS_PROBE_V1_READFIRST_2026_07_03`

Change:
- skip `dump_shadow_trace()` before priming and step motor reads
- first post-write read traffic is now the motor-speed readback itself
- if this improves readback, shadow-trace reads were materially disturbing the pipeline
- if it fails the same way, the defect is below the probe harness in command/response
  synchronization

Bench result:
- `READFIRST` did not improve readback
- skipping the shadow dump made the visible motor-speed reads collapse to all-zero
  lane/frame samples (`00 00 00 00 00 00 00 00`) with every motor read failing
- Pico USB COM9 was not visible during the run, so the run should not be treated as a
  complete Pico-internal counter proof
- after restoring Pico visibility with the v28 COM-alive UF2, the same `READFIRST`
  RW612 sequence returned to all-`ff` idle tails with intermittent real/stale values:
  motor3 often replayed `0x0000044C`, while motor1 mostly stayed zero or errored

Conclusion:
- pre-read shadow dumping is not the root cause; removing it made the transport worse
- the useful direction is not “bytes never arrive”; the remaining issue is ordering /
  pipeline position versus address/write commit

New Pico visibility artifact:
- `helper/pico_spi_slave_test_pico2_comalive_rspdelay_2026-07-02.uf2`

Pico marker:
- `dbal_motor_v1_onehot_v3_isoD_v28_pico2_comalive_rspdelay_2026-07-02`

Purpose:
- restore/verify Pico2 COM visibility while preserving the current rspdelay/exact8/txprogress
  firmware line
- next bench run should first confirm Pico USB startup text/heartbeat is visible before
  trusting RW612 readback logs

New staged RW612 read-order discriminator:
- `helper/pico2_alias_probe_readswap_v1_2026-07-03.elf`

Marker:
- `PICO2_ALIAS_PROBE_V1_READSWAP_2026_07_03`

Change:
- preserve the same writes, gaps, and skipped shadow dump from `READFIRST`
- read motor3 twice before reading motor1 twice

Expected discriminator:
- if the bad result follows the first read slot, the read/response pipeline ordering is
  the controlling issue
- if motor1 remains bad even when read second, the `0x5014` write/commit path remains
  suspect

Bench result:
- `READSWAP` falsified a pure first-read-slot explanation
- motor3 could still read back correctly when it was moved to the first read slot
  (for example step 1 returned `0x0000044C` twice)
- motor1 remained the weaker path even when read second
- step 2 cleanly replayed previous-step values (`motor3 -> 0x0000044C`, `motor1 -> 0x000001F4`)
  instead of the newly written values

Conclusion:
- the remaining fault is not just "whoever is read first loses"
- real bytes are arriving, but the dual-write interaction is still contaminating what
  later reads observe

New staged RW612 isolation discriminator:
- `helper/pico2_alias_probe_singlewrite_v1_2026-07-03.elf`

Marker:
- `PICO2_ALIAS_PROBE_V1_SINGLEWRITE_2026_07_03`

Change:
- keep the same Pico firmware and same secondary target
- remove the dual-write pattern from each step
- each step now writes only one motor speed value and then reads only that motor twice

Expected discriminator:
- if motor1 still fails while motor3 succeeds in single-write mode, the `0x5014` path is
  locally weaker even without cross-write interaction
- if both become cleaner, the dual-write sequencing itself is the controlling defect

Bench result:
- `SINGLEWRITE` did not recover the earlier real-byte/stale-value state
- both motor3-only and motor1-only steps collapsed to all-zero lane/frame samples
  (`00 00 00 00 00 00 00 00`) with every read failing

Conclusion:
- removing dual-write interaction alone is not sufficient when using the current
  shadow-skipped harness
- this branch has drifted away from the earlier useful `TRUEBYTE`-style state where
  real response bytes and coherent stale values were visible

New staged RW612 discriminator:
- `helper/pico2_alias_probe_singletrace_v1_2026-07-03.elf`

Marker:
- `PICO2_ALIAS_PROBE_V1_SINGLETRACE_2026_07_03`

Change:
- keep one-motor-at-a-time single-write steps
- restore the shadow-trace read path before each motor read, matching the earlier
  harness that produced real byte activity

Expected discriminator:
- if real/stale bytes return under `SINGLETRACE`, the shadow-trace path is part of the
  useful proven state and single-write isolation can be evaluated there
- if it still collapses to all-zero, the bench state or Pico firmware has diverged from
  the earlier useful baseline more fundamentally

Recovery decision:
- stop extending the degraded READFIRST/READSWAP/SINGLEWRITE/SINGLETRACE branch
- restore the last exact saved pair associated with proven real-byte activity:
  - RW612: `helper/pico2_alias_probe_v1_2026-07-02.elf`
  - Pico2: `helper/pico_spi_slave_test_pico2_txprogress_2026-07-02.uf2`

Reason:
- session notes already proved `pico2_txprogress` was the point where TX byte progress
  worked and RW612 reconstructed some reads again (`0x5120`, `0x5130`, `0x5014`, `0x5034`)
- later lifetime/trace/order experiments drifted into degraded `ff`/`00` branches that no
  longer preserve the earlier useful state

---

### 11. Exact 16-byte dual-write recovery probe was falsified
New local Pico probe:
- if `cs_end` saw exactly 16 buffered bytes and both 8-byte halves decoded as valid writes,
  process both frames instead of dropping the whole buffer

Artifact:
- `helper/pico_spi_slave_test_pico2_two_write_recover_2026-07-02.uf2`

Bench result:
- full regression back to total read failure
- RW612 again saw only all-`ff` lane-0 tails
- every read errored out

Conclusion:
- the remaining live bug is not safely explained by a simple “two back-to-back write frames
  got coalesced into one 16-byte cs_end buffer” model
- this recovery path perturbed normal read-command handling enough to be rejected as a good direction

---

### 12. RW612 write-gap probe confirmed master-side pacing matters
New local RW612 probe:
- keep the Pico on the saved live-read UF2 baseline
- insert `250 us` gaps between each write in the secondary alias probe sequence

Marker:
- `PICO2_ALIAS_PROBE_V1_WRITEGAP_2026_07_03`

Bench result:
- priming became fully correct for both motors
- step 1 became fully correct for both motors
- step 2 motor1 stayed correct, but motor3 degraded from `0x000001F4` to `0x000000F4`
- step 3 motor3 became correct at `0x000005A0`, but motor1 still replayed old step-2 value `0x0000044C`
- step 4 remained fully correct for both motors

Conclusion:
- master-side pacing directly influences whether Pico commits these writes cleanly
- the bug is not a uniform parser failure: a small added spacing fixed the earlier
  `0x5014 == 0` priming/step1 symptom and one half of step 3, but not all failing slots
- the remaining issue is now narrower still:
  - step 2 motor3 can lose the high byte of `0x000001F4` and become `0x000000F4`
  - step 3 motor1 can still replay the prior step value `0x0000044C`
- next probe direction: keep the timing hypothesis and widen only the speed-write gaps,
  not the entire parser/search surface

---

### 13. Speed-write gaps plus order swap tied the remaining fault to write position
New local RW612 probe:
- keep the added speed-write gaps
- swap the order of the two speed writes so motor3 speed is written before motor1 speed

Marker:
- `PICO2_ALIAS_PROBE_V1_SPEEDSWAP_2026_07_03`

Bench result:
- priming and step 1 were fully correct for both motors
- step 2 motor3 became correct at `0x000001F4`
- step 2 motor1 became wrong as `0x00001030`
- step 3 was fully correct for both motors
- step 4 motor3 stayed correct at `0x000000A5`
- step 4 motor1 became wrong as `0x000014A0`

Conclusion:
- the corruption followed the second speed-write slot strongly enough to move when the write order moved
- this is stronger evidence for a transaction-position / settling bug than for a register-specific bug tied only to `0x5014`

---

### 14. Retrying the second speed write was falsified and made corruption worse
New local RW612 probe:
- keep the swapped speed-write order and gaps
- immediately retry the second speed write with the same value

Marker:
- `PICO2_ALIAS_PROBE_V1_SPEEDRETRY_2026_07_03`

Bench result:
- priming motor1 stayed correct, but priming motor3 collapsed to stale `0x000000A5`
- step 1 motor1 became wrong as `0x000004F0`
- step 2 motor1 became wrong as `0x0000104C`
- step 2 motor3 stayed correct at `0x000001F4`
- step 3 motor1 stayed correct at `0x000000A5`
- step 3 motor3 became wrong as `0x000014A0`
- step 4 motor1 became wrong as `0x00001480`
- step 4 motor3 stayed correct at `0x000000A5`

Conclusion:
- retrying the second speed write is not a mitigation; it amplifies corruption and pollutes otherwise-correct slots
- the remaining defect is better modeled as sensitivity to the second speed-write position than as a simple transient first-attempt miss that a retry can heal
- next discriminator should move the duplicate to the first speed-write slot instead of the second

---

## Key Learnings
1. The stable motion baseline must remain isolated from readback experiments.
2. Short, decisive logs are worth more than huge dumps once the problem is local.
3. Pico was in fact generating full valid `a0 addr 01 payload...` reply frames on a byte lane.
4. RW612 provisional interleaved fallback was useful for discovery but unsafe as a truth source for motor-speed reads.
5. The detached RW612 tree was the actual bench source; patching only the workspace copy can mislead diagnostics.
6. Lane-0 compact fingerprints were the turning point that shifted the root cause from Pico write corruption to RW612 reply acceptance.
7. Exact byte-lane acceptance is necessary, but accepting rotated/header-contaminated variants too early produces false-positive first reads.
8. Reconstructed Pico rollback builds are not reliable substitutes for exact saved UF2 artifacts once several interacting late-session probes have accumulated.
9. The saved UF2 `pico_spi_slave_test_pico2_nodup_flushrearm_nosniffshort_rxguard_2026-07-02.uf2` is the last confirmed Pico2 artifact that restores live reads on the current bench.
10. The remaining live failure shape is narrower than general parser failure: motor1 `0x5014` can stay pinned at zero early, and step 3 can replay step-2 values cleanly on both motors.
11. The exact 16-byte dual-write recovery probe is falsified and should not be resumed as the default next path.
12. Small master-side write gaps materially improve the behavior, which confirms transaction pacing is part of the remaining defect.
13. After `250 us` write gaps, the residual failures became even more specific: step 2 motor3 can drop the high byte (`0x01`) of `0x000001F4`, and step 3 motor1 can still replay `0x0000044C`.
14. Swapping speed-write order moved the corruption with the second speed-write slot, which is stronger evidence for write-position sensitivity than for a single bad register address.
15. Retrying the second speed write is falsified as a mitigation; it makes the corruption worse and spreads it into additional slots.

---

## Current Code State

### Pico2 firmware
Latest meaningful saved Pico2 artifact for this bench:
- `helper/pico_spi_slave_test_pico2_nodup_flushrearm_nosniffshort_rxguard_2026-07-02.uf2`

Late-session experimental artifacts that were explicitly falsified:
- `helper/pico_spi_slave_test_pico2_rxguard_revertchunk_2026-07-02.uf2`
- `helper/pico_spi_slave_test_pico2_two_write_recover_2026-07-02.uf2`

Notable artifacts:
- `helper/pico_spi_slave_test_pico2_identity8_rxguard_2026-07-02.uf2`
- `helper/pico_spi_slave_test_pico2_rearm_txrestart_2026-07-02.uf2`
- `helper/pico_spi_slave_test_pico2_readrestart_eachcs_2026-07-02.uf2`

### RW612 firmware
Active experimental source tree:
- `dbus_porting_BA_known_good_2fd321d/zephyr_dbus_driver`

Staged flash artifact path:
- `zephyr_dbus_driver/build_local/zephyr/zephyr.elf`

Latest staged marker:
- `PICO2_ALIAS_PROBE_V1_SPEEDRETRY_2026_07_03`

---

## Current Hypothesis
The remaining live defect is now best treated as Pico-side write ownership / commit timing,
not RW612 reply extraction.

The strongest surviving hypothesis after the late-session bench runs is:

- some Pico-side exact 8-byte write transactions are not reaching authoritative commit at `cs_end`
- this is most visible in two places:
  - without added pacing, the first motor1 speed write to `0x5014` can remain invisible, so reads return `0x00000000`
  - with added pacing, the corruption can move with the second speed-write slot when write order changes
  - retrying that second speed-write slot makes the corruption worse instead of curing it
- the fault now looks more like a write-position / settling problem around the paired speed writes than a general register-store bug

---

## Next Bench Discriminator
Resume from the last saved live-read Pico artifact:
- `helper/pico_spi_slave_test_pico2_nodup_flushrearm_nosniffshort_rxguard_2026-07-02.uf2`

Keep the RW612 image at marker:
- `PICO2_ALIAS_PROBE_V1_SPEEDRETRY_2026_07_03`

Most useful next discriminator for the next session:
- keep the swapped order and gaps, but move any duplicate/retry from the second speed-write slot to the first speed-write slot
- if corruption stays attached to the second slot, continue pursuing a paired-write position / settling explanation
- if corruption follows the duplicated write regardless of slot, then the duplicate itself is destabilizing the Pico-side path
- avoid reopening RW612 parser experiments or buffered multi-frame recovery paths unless
  the live-read behavior changes again

---

## Practical Summary
This session first narrowed the problem from broad corruption to parser acceptance, and then
late-session bench work narrowed it again to a Pico-side write-commit issue on the isolated
secondary path.

- RW612 exact-lane acceptance is good enough to expose real Pico behavior.
- The last known good diagnostic Pico artifact is `pico_spi_slave_test_pico2_nodup_flushrearm_nosniffshort_rxguard_2026-07-02.uf2`.
- The remaining live failure is not random: after a `250 us` write-gap probe, the residuals narrowed to `step2 motor3 = 0x000000F4` instead of `0x000001F4`, and `step3 motor1 = 0x0000044C` instead of `0x000000A5`.
- Swapping the speed-write order moved the bad slot with the second write, and retrying that second write made the corruption worse.
- Reconstructed rollback UF2s and the exact 16-byte dual-write recovery probe both regressed to dead all-`ff` reads and should not be treated as the active path forward.

---

## Continuation Update — Pico RX Reframe

Later readback experiments moved away from RW612 parser tuning and back down to the
Pico2 transport layer.

Important changes and fixes after the earlier speed-gap work:
- RW612 `DBCDRV_readReg32()` dummy retries were fixed so retries send pure zero dummy
  clocks instead of repeating full READ headers.
- RW612 compact read logging now records first and last raw 8-byte frames.
- Pico diagnostic fake read/default-TX overrides were disabled because they emitted
  non-`0xA0` frames and guaranteed RW612 extraction failure.
- Pico `spi_slave_force_queue_current_tx_frame()` was fixed to poll `spi_is_writable()`
  before advancing `tx_index`; blind writes could claim queued bytes that never entered
  the PL022 TX FIFO.
- Pico USB CDC / heartbeat logging must stay enabled for this diagnostic phase; losing
  COM visibility blocks the only independent view of Pico-side counters.

Latest decisive Pico USB log evidence:
- GPIO sniffer sees plausible RW612 command frames, including examples such as
  `40 50 14 01 00 00 00 00`, `40 50 34 01 00 00 00 00`,
  `60 50 14 01 a5 00 00 00`, and `40 51 20 01 00 00 00 00`.
- CS and SCK are definitely seen: `csf`/`csr` and `scke` rise steadily.
- Authoritative PL022 exact-frame decode does not run: `f8=0`, `f8ok=0`,
  `f8w=0`, `f8r=0` across the log.
- No read response is prepared: `lrrn=0`, `lrra=0x0000`, `txd=ffffffffffffffff`.
- Repeated `CS_START dropping stale rx_index=1` shows the PL022 RX path is usually
  retaining only one stale byte, while the GPIO sniffer sometimes has a complete frame.

Conclusion:
- The current blocker is not RW612 extraction and not Pico register storage.
- The current blocker is Pico2 RW612-to-Pico command decode: the PL022 RX-FIFO path is
  not forming exact 8-byte register frames even though the GPIO sniffer sees MOSI/SCK/CS.

Current probe implemented:
- In `pico_spi_slave_test/main.c`, exact 8-byte sniffer captures now get copied into
  `rx_frame_raw` and passed through the existing `process_rx_frame()` validator.
- 7-byte near-frames remain diagnostic-only and must not mutate state.
- Firmware marker:
  `dbal_motor_v1_onehot_v3_isoD_v24_pico2_exact8sniff_2026-07-02`
- Staged UF2:
  `helper/pico_spi_slave_test_pico2_exact8sniff_2026-07-02.uf2`

Next bench discriminator:
- Flash only the new Pico2 UF2 above; keep the current RW612 ELF unchanged.
- Expected pass signal: Pico heartbeat should show `f8 > 0`, `f8ok > 0`, and reads should
  start preparing responses (`f8r > 0`, `lrrn > 0`, `txd` no longer all `ff`).
- If `f8` rises but `f8ok` stays zero, the sniffer byte order/phase is still wrong.
- If `f8ok` and `f8r` rise but RW612 still sees all `ff`, return to Pico TX/MISO timing.

Bench result from `pico2_exact8sniff`:
- The fallback worked for command decode: heartbeat rose to examples like
  `f8=329`, `f8ok=26`, `f8w=18`, `f8r=8`, `lrrn=8`.
- Pico prepared real responses such as `a0 50 14 01 f4 01 00 00` and
  `a0 51 20 01 34 50 00 00`.
- RW612 still failed reads because it mostly received byte-0-only progress:
  `lane0 raw=a0 a0 a0 a0 a0 a0 a0 a0` and frames like
  `a0 00 00 00 00 00 00 00`.

New conclusion:
- RX command decode is no longer the blocker under the exact-8 sniffer fallback.
- The active blocker has moved to TX response delivery: pending read responses are being
  restarted or underfilled so RW612 retry clocks collect repeated `0xA0` byte 0 instead
  of progressing through bytes 1..7.

Current follow-up probe:
- `spi_slave_force_queue_current_tx_frame()` now preserves `tx_index` for pending read
  responses, matching the historical cumulative retry strategy.
- It only resets `tx_index` when no read response is pending or the response has already
  completed.
- Firmware marker:
  `dbal_motor_v1_onehot_v3_isoD_v25_pico2_txprogress_2026-07-02`
- Staged UF2:
  `helper/pico_spi_slave_test_pico2_txprogress_2026-07-02.uf2`

Next bench discriminator:
- Flash `pico_spi_slave_test_pico2_txprogress_2026-07-02.uf2` and keep RW612 unchanged.
- Expected pass signal: RW612 lane0 should stop being all `a0` and start showing the
  response tail bytes (`50 14 01 ...` / payload bytes) across retry attempts.
- If lane0 progresses but ordering is shifted, return to RW612 cumulative extractor rules.
- If lane0 remains all `a0`, the PL022 TX FIFO is not retaining/accepting progress across
  CS pulses and the next probe should change rearm/preload timing rather than RX decode.

Bench result from `pico2_txprogress`:
- TX progress preservation worked: Pico `txi` advanced across CS pulses (`0..7`) instead
  of restarting at byte 0 every time.
- RW612 started getting successful reconstructed reads again, for example
  `0x5120 -> 0x00005034`, `0x5130 -> 0x00005034`, `0x5014 -> 0x000001f4`,
  and `0x5034 -> 0x000000a5`.
- Remaining failure shape is now stale/shifted response lifetime rather than byte-0-only
  TX: old response fragments leak into later reads, and the first direct read can still
  fail before the second succeeds.

New conclusion:
- RX decode is functional and TX byte progress is functional enough for RW612 cumulative
  reconstruction.
- Pending read responses live too long: after a full 8-byte response has been queued,
  `pending=1` persists and the same response can replay into later register reads.

Current follow-up probe:
- Retire a pending read response once `tx_index >= FRAME_SIZE` in both TX queue helpers.
- On retirement, clear `tx_read_response_pending`, clear `tx_frame_prequeued`, restore the
  default TX pattern, and reset `tx_index` for idle traffic.
- Firmware marker:
  `dbal_motor_v1_onehot_v3_isoD_v26_pico2_rsponce_2026-07-02`
- Staged UF2:
  `helper/pico_spi_slave_test_pico2_rsponce_2026-07-02.uf2`

Next bench discriminator:
- Flash `pico_spi_slave_test_pico2_rsponce_2026-07-02.uf2`, keep RW612 unchanged.
- Expected pass signal: fewer stale reads where a later address returns the previous
  response, and fewer first-read failures followed by second-read success.
- If reads regress to all `ff`, response retirement is too early and should move from
  “queued full frame” to “observed CS after full queue”.