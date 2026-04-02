# DBus Integration Execution Plan (Zephyr DBus Driver → RW612 SPI Master → Pico SPI Slave)

## Objective
Move from register-level SPI validation to full DBus app-layer integration while preserving the known-good motor baseline (20/20 PASS, physical motor movement confirmed).

## Current Baseline (as of 2026-04-02)
- RW612 side:
  - Low-level DBus driver path is active (`DBCDRV_writeReg32/readReg32`).
  - Repeatability tests pass (20/20) for scratchpad + motor0 enable/speed/feedback/status.
  - DBAL bootstrap is verified at runtime.
  - A motor service abstraction layer exists and is used by `main.c` for motor test flow.
- Pico side:
  - Register map + motor GPIO/PWM backend is working on GP12–GP15.
  - Still expects the current 8-byte register-oriented SPI protocol.
- Gap:
  - Full DBAL-over-wire parsing is not yet implemented on the Pico side.

## Integration Scope
- In scope:
  1. Initialize DBAL app layer in runtime flow.
  2. Register service handlers for motor and diagnostics.
  3. Route motor commands via DBAL message semantics (service/command IDs), not only raw register ops.
  4. Verify end-to-end with repeatability tests.
- Out of scope for Phase 1:
  - Multi-motor scaling (motor2..motor4) beyond one canonical service path.
  - Ultrasonic sensor integration.

## Phase Plan

### Phase 1 — DBAL Bootstrap in App (MVP foundation)
- Tasks:
  - Add DBAL bootstrap function in `zephyr_dbus_driver/src/main.c`.
  - Call `dbal_init()` at startup.
  - Register at least one service handler (test/diagnostic).
  - Keep existing register-level test loop intact as fallback baseline.
- Acceptance criteria:
  - Build passes.
  - Startup logs confirm DBAL init + service registration.
  - Existing repeatability loop still runs.

### Phase 2 — DBAL Motor Service Contract
- Tasks:
  - Define service IDs/command IDs for motor enable, set speed, read feedback, read status.
  - Add encode/decode helpers for payload schema.
  - Implement sender/receiver routing with ACK/NACK behavior.
- Acceptance criteria:
  - Motor commands can be expressed via DBAL service commands.
  - Invalid payloads rejected with NACK.

Status:
- In progress / partial completion on RW612 side.
- Service abstraction added and build-verified.
- Wire-level DBAL handling on Pico side still pending.

### Phase 3 — End-to-End DBAL Path Activation
- Tasks:
  - Switch test flow from raw register calls to DBAL service calls.
  - Maintain readback assertions for status/feedback.
  - Keep a compile-time or runtime fallback to raw-register mode during bring-up.
- Acceptance criteria:
  - DBAL-path test run succeeds repeatedly.
  - Fallback mode remains available for debug.

### Phase 4 — Hardening
- Tasks:
  - Retry/timeout tuning.
  - Error propagation and diagnostics counters.
  - Stress/repeatability sweep.
- Acceptance criteria:
  - Stable under repeated run cycles with clear fault logs.

## Estimated Effort
- Phase 1: 0.5–1 day
- Phase 2: 1–2 days
- Phase 3: 1–2 days
- Phase 4: 1–2 days
- Total: ~4–7 working days for robust integration (MVP can happen sooner).

## Hardware Dependency by Stage
- No hardware required:
  - Service contract definition, compile-time wiring, unit-level marshaling helpers.
- Hardware required (master+slave connected):
  - End-to-end DBAL transport validation.
  - Timing/retry behavior and repeatability confirmation.
  - Physical motor behavior verification.

## Risks / Watch Items
- DBAL app-layer framing must match what peer expects; frame mismatch can look like transport failure.
- Existing SPI abstraction and low-level DBCDRV usage can contend if both paths transmit concurrently.
- Keep raw-register fallback until DBAL path is fully stable.

## Change Log
- 2026-03-20: Initial plan created.
- 2026-04-02: Phase 1 runtime-verified; TX-thread-only send path fixed; Phase 2 motor service foundation added on RW612 side.
