# Session Log: Pico SPI Slave Frame Alignment Fix
**Date**: March 18, 2026  
**Project**: Zephyr DBus Driver - SPI Protocol Porting (RW612 Master ↔ Pico RP2040 Slave)  
**Goal**: Fix PRE_MOTOR_V1 validation test failures caused by SPI frame misalignment

---

## Known-Good Baseline (as validated on March 19, 2026)

- **Branch / Snapshot**: `pico` at commit `264c7f1` (full runnable workspace snapshot)
- **Focused source fix commit**: `c868539` (`pico_spi_slave_test/main.c`, `zephyr_dbus_driver/src/dbus_driver.c`)
- **RW612 image**: `build/zephyr/zephyr.elf`
- **Pico firmware**: `cs_aligned_tx_v2_noblk` with SERIAL_ROL1 TX compensation in `prepare_tx_frame_wire()`
- **SPI mode**: MODE0 (`CPOL=0`, `CPHA=0`)
- **Observed link behavior**: response marker often arrives as `0x20` (bit7-cleared variant of `0xA0`)
- **RW612 matcher requirement**: marker-tolerant fallback enabled, including shifted-header reconstruction path
- **Validation status**:
    - `PRE_MOTOR_V1`: repeated `PASS`
    - `MOTOR0_TOGGLE_V1`: repeated complete cycles with correct feedback (e.g., 300 / 800 / 1200)

---

## Problem Statement

### Initial Symptom
**PRE_MOTOR_V1 test results showed**:
- Write commands: Partially working
- Read responses: Consistently returning wrong data or zeros
- Error pattern: "no matching response after dummy retries"

**Root issue**: Responses had incorrect frame alignment with one-byte offset (or complete zeros on subsequent attempts)

### Example from First Test Log
Write 0x12345678 to addr 0x001C:
- Expected response header: `A0 00 1C 01 ...`
- Actual: `00 20 00 1C 01 ...` (byte-shifted) or `00 00 00 00 ...` (zeros)

---

## Investigation Phase

### Step 1: Search for Pico SPI Slave Examples
**Queries searched**:
- GitHub: `pico spi slave` (8 results found)
- Repositories analyzed:
  - `giggleware/spi_pico_slave` - Uses standard `spi_write_read_blocking()`
  - Pico SDK source (`hardware/spi/spi.c`) - Shows standard preload pattern
  
**Key finding**: All working examples pre-load the **entire TX buffer BEFORE the transaction starts**, not incrementally during the transaction.

### Step 2: Code Review - Pico SPI Slave Handler
**File**: `pico_spi_slave_test/main.c`

**Original `service_spi_frame()` logic**:
```c
// Reset indices based on byte count
if (rx_index == FRAME_SIZE) {
    process_rx_frame();
    rx_index = 0;
    tx_index = 0;  // ← PROBLEM: Reset timing wrong
}

// Load TX FIFO unconditionally
while (spi_is_writable(spi) && tx_index < FRAME_SIZE) {
    hw->dr = tx_frame_wire[tx_index];
    tx_index++;
}
```

**Issues identified**:
1. **Frame reset tied to byte count, not CS boundaries** → Frame indices reset at wrong time relative to actual chip-select boundaries
2. **TX FIFO loading not guarded by CS state** → TX data being loaded during idle periods
3. **`tx_index` not resetting when new response prepared** → FIFO never refilled with updated response data

---

## Fixes Applied

### Fix 1: CS-Based Frame Synchronization (Commit 01fb4a9)

**Problem**: Indices were reset when RX collected 8 bytes, not when CS actually went low.

**Solution**:
```c
static bool last_cs_state = true;

// Detect CS high→low transition
if (!last_cs_state && current_cs_state) {
    rx_index = 0;
    tx_index = 0;
}
last_cs_state = current_cs_state;

// Only preload during active CS
if (current_cs_state) {
    while (spi_is_writable(spi) && tx_index < FRAME_SIZE) {
        hw->dr = tx_frame_wire[tx_index];
        tx_index++;
    }
}
```

**Impact**: Frame alignment now syncs to actual CS boundaries instead of byte count heuristic.

---

### Fix 2: Corrected Inverted CS Logic (Commit 29f808e)

**Problem**: Logic was backwards! 
- `cs_is_active()` returns `true` when CS is LOW (active)
- Code was checking `last_cs_state && !current_cs_state` (active→idle)
- Should be `!last_cs_state && current_cs_state` (idle→active)

**Original buggy code**:
```c
if (last_cs_state && !current_cs_state) {  // WRONG: active→idle
    rx_index = 0;
    tx_index = 0;
}
if (current_cs_state == false) {  // WRONG: "NOT active" = disabled preload!
    while (spi_is_writable(spi) && tx_index < FRAME_SIZE) { ... }
}
```

**Result**: TX FIFO **never preloaded** → Pico sent all zeros

**Fixed code**:
```c
if (!last_cs_state && current_cs_state) {  // CORRECT: idle→active
    rx_index = 0;
    tx_index = 0;
}
if (current_cs_state) {  // CORRECT: preload when CS IS active
    while (spi_is_writable(spi) && tx_index < FRAME_SIZE) { ... }
}
```

**Impact**: Restored ability to send response data.

---

### Fix 3: Reset `tx_index` When New Response Prepared (Commit 388aab3)

**Problem**: When `process_rx_frame()` prepared a new response:
```
Frame 1: rx_index goes 0→8, tx_index goes 0→8 for default pattern
Response prepared by process_rx_frame()
tx_index STILL at 8! ← STUCK
Next preload: while(writable && tx_index < 8) is FALSE
TX FIFO never gets new response → All zeros sent
```

**Solution**: Reset `tx_index = 0` after every `prepare_tx_frame_wire()` call:
```c
// In process_rx_frame() for writes:
set_default_tx_pattern();
tx_index = 0;  // ← Allow FIFO refill

// In process_rx_frame() for reads:
prepare_tx_frame_wire();
tx_index = 0;  // ← Allow FIFO refill

// In process_rx_frame() for invalid frames:
set_default_tx_pattern();
tx_index = 0;  // ← Allow FIFO refill
```

**Impact**: TX FIFO now refilled every time response changes, enabling sending of read responses.

---

## Commits Summary

| Commit | File | Change | Status |
|--------|------|--------|--------|
| `01fb4a9` | pico/main.c | Add CS-based frame sync, track `last_cs_state` | ✅ Built |
| `29f808e` | pico/main.c | Fix inverted CS logic (both conditions) | ✅ Built |
| `388aab3` | pico/main.c | Reset `tx_index=0` on response prepare | ✅ Built |

**GitHub branch**: `origin pico` - All commits pushed

---

## Current Status

### Hardware Setup
- **Master**: RW612 (Cortex-M33) - Latest firmware with "latest match" response strategy
- **Slave**: Raspberry Pi Pico (RP2040) - Latest UF2 with all 3 fixes applied
- **Interface**: SPI MODE0 (CPOL=0, CPHA=0), 1 MHz
- **Link transform**: Serial ROR1 on MISO (due to CPHA mismatch) → Pico pre-compensates with serial ROL1

### Latest Test Run (Before Fixes Applied)
**PRE_MOTOR_V1 result**: FAIL (11/11 checks failed due to frame offset/zeros)
- Scratchpad patterns: Read responses all zeros
- Motor enable/disable: Read responses all zeros  
- Speed feedback: All zeros
- Status: All zeros

### Expected After Fixes
With all 3 fixes applied, PRE_MOTOR_V1 should now:
1. Write scratchpad patterns ✓ (writes were mostly working)
2. **Read scratchpad back correctly** (Fixed by commit 388aab3)
3. **Motor enable/disable cycle work** (Fixed by frame alignment + tx_index reset)
4. **Speed feedback reads work** (Fixed by tx_index reset allowing response preload)
5. **Status reads work** (Fixed by tx_index reset)

**Next action**: Flash Pico with UF2, power cycle, run PRE_MOTOR_V1 test

---

## Technical Deep Dive

### SPI Slave Protocol Overview
**Frame structure**: 8 bytes
- Byte 0: Command/Marker (0x40=read, 0x60=write, 0xA0=response)
- Bytes 1-2: Address (high, low)
- Byte 3: Length (0x01)
- Bytes 4-7: Data (little-endian uint32_t)

### Link-Level Transform
**Hardware CPHA mismatch requires special handling**:
- CPHA=0 on both sides but link applies 1-bit serial RIGHT shift to MISO
- Fix: Pico pre-compensates with 1-bit serial LEFT shift
  - `tx_wire[i] = (desired[i]<<1) | (desired[i+1]>>7)` for i=0..6
  - `tx_wire[7] = desired[7]<<1`
  - RW612 receives `desired[1..7]` correctly; `desired[0]` uncorrectable

### Frame Alignment Critical Timing
1. **CS goes LOW** → Indices reset (Frame start)
2. **Incoming bytes** → Collected into `rx_frame_raw[]`
3. **After 8 bytes** → `process_rx_frame()` decodes command and prepares response
4. **TX FIFO refill** → `tx_index` reset to 0, response bytes loaded on next `service_spi_frame()` call
5. **Master dummy exchanges** → Read data sent on MISO during dummy clock periods
6. **CS goes HIGH** → End of transaction

---

## Lessons Learned

1. **CS synchronization is critical** in SPI slave protocols
   - Never use byte count as proxy for transaction boundaries
   - Always sync to actual CS edges when managing state

2. **SPI slave TX FIFO management is tricky**
   - Pre-loading entire buffer before transaction (like SDK) is simpler
   - Incremental load requires careful index management and reset points

3. **Test early with simple examples**
   - GitHub search for "pico spi slave" found working reference implementations
   - Comparing against reference code revealed the pattern-based reset issue

4. **Logic inversion bugs are hard to catch**
   - Boolean signal polarity (CS is active-LOW) can lead to inverted conditions
   - Always double-check logic when signal semantics are counterintuitive

5. **Response preload index state is ephemeral**
   - Must reset whenever a NEW response is prepared
   - Not just at transaction boundaries or frame completion

---

## Files Modified This Session

```
pico_spi_slave_test/main.c
├─ Line 49: Added static bool last_cs_state tracking
├─ Line 352-392: Rewrote service_spi_frame() with CS detection
├─ Line 282: Added tx_index=0 after set_default_tx_pattern() in write path
├─ Line 301: Added tx_index=0 after prepare_tx_frame_wire() in read path
└─ Line 304: Added tx_index=0 after set_default_tx_pattern() in invalid path
```

---

## Next Steps (Pending User Action)

1. ✅ Build & UF2 copy complete → `/mnt/d/pico_spi_slave_test.uf2` ready
2. ⏳ **Flash Pico** with new UF2 (hold BOOTSEL, copy file, auto-reboot)
3. ⏳ **Power cycle both boards** (Pico + RW612)
4. ⏳ **Run PRE_MOTOR_V1 validation test** and share output log
5. 🎯 **Expected result**: All 11 checks PASS (or identify any remaining issues)

---

## Reference Materials

### SPI Slave Frame Sync Key Points
- **Transaction boundary**: CS LOW
- **Frame collection**: First 8 bytes after CS goes low
- **Response preparation**: After complete frame received
- **Response transmission**: During dummy exchanges (RW612 clocks after initial command)
- **Index management**: Reset rx_index/tx_index only at CS transitions, NOT byte count

### Serial Bit Transform Formula
```
ROR1 (Right rotate by 1):  bit = (bit >> 1) | (bit << 7)
ROL1 (Left rotate by 1):   bit = (bit << 1) | (bit >> 7)

Link applies ROR1 to MISO bits
Pico applies ROL1 to compensate
Result: RW612 receives correct byte values
```

---

**Session completed**: All code fixes implemented, built, and committed.  
**Deployment status**: Awaiting user to flash Pico and retest.

---

## Continuation: March 19, 2026 (Failures → Final Stabilization)

### Context at Start of Day
- Pico firmware already moved to non-blocking logging (`cs_aligned_tx_v2_noblk`)
- RW612 matcher already had deterministic matching + marker-tolerant fallback
- PRE_MOTOR_V1 was close but still failing on two critical reads in some runs

### Observed Failing Patterns (Early March 19)
1. `scratchpad pattern 2 readback 0xaaaaaa2a (expected 0xaaaaaaaa)`
2. `motor0 speed feedback readback 0x00008430 (expected 0x000004b0)`

Also observed in RW612 logs:
- Valid response frames arriving with marker `0x20` (not only `0xA0`)
- Example frame for speed path: `a0 d0 04 01 30 84 00 00`

---

## Root Cause Analysis (March 19)

### Key Discovery
Both remaining corruptions had the same root cause in Pico TX pre-compensation:

- Pico was using **per-byte ROL1** when preparing `tx_frame_wire[]`
- The physical link behavior on MISO is effectively **serial ROR1 across the whole byte stream** (inter-byte carry), not independent per-byte rotate

### Why This Caused Exactly Those Wrong Values

1. `0x000004b0` became `0x00008430`
     - Byte carry between `0xb0` and `0x04` was handled incorrectly by per-byte ROL1

2. `0xaaaaaaaa` became `0xaaaaaa2a`
     - Carry from length byte/data boundary was lost in the first payload byte

---

## Fixes Implemented on March 19

### Fix A: Pico TX Compensation Updated to SERIAL_ROL1
**File**: `pico_spi_slave_test/main.c`

Changed `prepare_tx_frame_wire()` from per-byte:
- `tx_frame_wire[i] = rol1(tx_frame_desired[i])`

To serial cross-byte compensation:
- `W[i] = (desired[i] << 1) | (desired[i+1] >> 7)` for `i < FRAME_SIZE-1`
- `W[last] = desired[last] << 1`

This made Pico TX compensation match the observed link behavior.

### Fix B: RW612 Shifted-Header Matcher Made Marker-Tolerant
**File**: `zephyr_dbus_driver/src/dbus_driver.c`

Shifted-header detection path was widened from strict marker `0xA0` to masked marker check (`0xA0/0x20` equivalent under bit7 corruption) for the one-byte-shift reconstruction path.

Reason: some valid shifted responses arrived with marker `0x20` and were previously missed in that branch.

---

## Build + Flash Cycle

### Pico
- Rebuilt Pico firmware successfully (`pico_spi_slave_test` target)
- UF2 copied to Windows drive for flashing (`/mnt/d/pico_spi_slave_test.uf2`)

### RW612
- Rebuilt Zephyr image successfully
- `build/zephyr/zephyr.elf` flashed for retest

---

## Validation Progression (March 19)

### Intermediate Result
- Pico serial logs showed correct register behavior for critical addresses:
    - `0x001c` scratchpad read/write correct
    - `0x5004` setpoint readback correct
    - `0x5008` feedback tracked setpoint correctly while enabled
    - `0x500c` status returned `0x00000003`

### First near-pass after Pico fix
- PRE_MOTOR_V1 still failed once due to shifted-header + `0x20` marker case in RW612 scratchpad path

### After RW612 shifted-header marker-tolerant patch
- PRE_MOTOR_V1: **PASS**
- MOTOR0_TOGGLE_V1: completed cycles with correct feedback/status

### Repeated Confirmation Runs
Multiple repeated runs (as shared in session output) all showed:
- `Main: PRE_MOTOR_V1 finished: PASS`
- `Motor Toggle [MOTOR0_TOGGLE_V1]: Complete.`
- Correct feedback values for cycle speeds (e.g. 300, 800, 1200)
- No recurrence of `0xaaaaaa2a` or `0x00008430`

Conclusion: communication path and protocol handling stabilized for current setup.

---

## Git History for Final Working State

### Focused fix commit
- **`c868539`**
- Message: `Fix SPI serial-shift compensation and marker-tolerant response matching`
- Includes source changes in:
    - `pico_spi_slave_test/main.c`
    - `zephyr_dbus_driver/src/dbus_driver.c`

### Full runnable snapshot commit (requested)
- **`264c7f1`**
- Message: `Snapshot full runnable workspace state`
- Includes full workspace/build state so pulling branch reproduces current runtime environment

### Remote status
- Branch: `pico`
- Pushed to: `origin/pico`
- Working tree confirmed clean after snapshot commit

---

## Final Session Outcome

✅ Remaining protocol corruptions resolved  
✅ PRE_MOTOR_V1 passing repeatedly  
✅ MOTOR0_TOGGLE_V1 passing repeatedly  
✅ Code committed and pushed (focused + full snapshot)  

Project is ready to proceed from SPI/protocol bring-up into controlled motor bring-up steps.

---

## Continuation: March 19, 2026 (Motor1 Pin Mapping + 3.3V Logic Baseline)

### Electrical Baseline Confirmed
- TB6612FNG **logic supply** `VCC = 3.3V` (from buck converter, adjusted from 5V)
- TB6612FNG **motor supply** `VM = 9V` (battery)
- **Common ground required** between Pico, TB6612, and power supplies

Rationale: Pico GPIO are 3.3V logic. Driving TB6612 at `VCC=3.3V` guarantees valid logic-high margins without level shifters.

### Pin Mapping Locked (Current Wiring + Firmware)
- **SPI0 communication (kept unchanged):**
    - GP16 = SPI0 RX
    - GP17 = SPI0 CSn
    - GP18 = SPI0 SCK
    - GP19 = SPI0 TX
- **Motor channel naming (future-proofed):** `motor1..motor4`
- **Active motor control pins (motor1):**
    - GP15 = `PWMA`
    - GP14 = `STBY`
    - GP13 = `AIN1`
    - GP12 = `AIN2`

This avoids any SPI pin conflict while reserving motor control on GP12–GP15.

### Firmware Updates Applied
- `pico_spi_slave_test/main.c`
    - Added explicit motor GPIO/PWM config for GP12–GP15
    - Added runtime drive logic tied to existing motor registers:
        - `STBY` asserted only when enabled
        - `AIN1/AIN2` set from speed sign (direction)
        - `PWMA` duty set from speed magnitude
    - Added startup serial prints that echo active SPI and Motor1 pin map
    - Updated runtime logs to use `motor1` naming
- `pico_spi_slave_test/CMakeLists.txt`
    - Added `hardware_pwm` link dependency

### Build Verification
- Rebuild in `pico_spi_slave_test/build` completed successfully:
    - `100% Built target pico_spi_slave_test`

### Baseline Status
Motor bring-up baseline is now documented with:
1. 3.3V logic-safe electrical setup
2. Non-conflicting SPI + motor pin assignment
3. Firmware-level enforcement of mapped pins

---

## Continuation: March 19, 2026 (Motor1 Physical Bring-Up — COMPLETE)

### Milestone Achieved
Full end-to-end motor control test completed with **physical motor rotation confirmed**.

### Test Configuration
- Firmware: `cs_aligned_tx_v2_noblk_motorpins`
- RW612 test sequence: `PRE_MOTOR_V1` (SPI validation) + `MOTOR0_TOGGLE_V1` (3-speed cycle)
- TB6612FNG wired: GP15→PWMA, GP14→STBY, GP13→AIN1, GP12→AIN2
- Power: VCC=3.3V, VM=9V, common GND

### RW612 Repeatability Result
```
Total runs: 20
PASS:       20
FAIL:       0
Overall:    PASS
```

### Motor Physical Behavior Confirmed
- Motor **physically rotated** at all three commanded speeds: 300, 800, 1200
- Direction and enable/disable actuation worked correctly per cycle
- Motor stopped cleanly on each `enable=0` command

### MOTOR0_TOGGLE_V1 Per-Cycle Results (representative — run 18/20)
| Cycle | Speed | Status     | Feedback | Enable→0 |
|-------|-------|------------|----------|----------|
| 1     | 300   | 0x00000003 | 300      | ✅       |
| 2     | 800   | 0x00000003 | 800      | ✅       |
| 3     | 1200  | 0x00000003 | 1200     | ✅       |

### System State After Completion
- SPI protocol: stable, no data corruption across 20 runs
- Motor register interface: all write/readback values match expected
- PWM output: correctly mapped from speed magnitude
- Direction pins: AIN1/AIN2 driven correctly
- STBY: asserted on enable, de-asserted on disable

### Next Steps
- Git commit: firmware + CMakeLists + session log
- Suggested commit message: `feat: motor1 GPIO/PWM bring-up on GP12-GP15, 20/20 PASS with physical rotation`
- Proceed to multi-motor support (motor2–motor4) or DBus integration layer
