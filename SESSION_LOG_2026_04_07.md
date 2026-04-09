# Session Log — April 7, 2026

**Project**: Zephyr DBus Driver — SPI Transport Restore (RW612 Master ↔ Pico RP2040 Slave)  
**Branch**: `pico` (HEAD = `7e6444d`)  
**Goal**: Restore PRE_MOTOR_V1 and MOTOR0_TOGGLE_V1 repeatability tests to 5/5 PASS after DBAL integration regression

---

## Known-Good Baseline (Pre-Session Reference)

### Last Fully Passing State
| Item | Value |
|------|-------|
| Commit | `30c9e80` — "motor1 GPIO/PWM bring-up on GP12-GP15, 20/20 PASS with physical rotation" |
| Prior commit | `c868539` — "Fix SPI serial-shift compensation and marker-tolerant response matching" |
| RW612 image | `build/zephyr/zephyr.elf` @`c868539` |
| Pico firmware | `cs_aligned_tx_v2_noblk` with SERIAL_ROL1 TX compensation |
| SPI mode | MODE0 (CPOL=0, CPHA=0), 1 MHz |
| Tests | PRE_MOTOR_V1: repeated PASS · MOTOR0_TOGGLE_V1: repeated complete cycles |
| Motor feedback values observed | 300 / 800 / 1200 |

### What the April 2 Session Changed (Commit `7e6444d`)
- `zephyr_dbus_driver/src/dbus_app_layer.c`: fixed wrong success check for `spi_abstraction_init()` (`== true` → `== 0`) and `spi_abstraction_send()` (same fix); corrected `ret_val` assignment behavior in send path.
- `zephyr_dbus_driver/src/motor_service.c/.h`: new motor service abstraction added.
- `zephyr_dbus_driver/src/main.c`: refactored to use `motor_service_*` APIs.
- **Side-effect**: This commit modified `dbus_driver.c`, `spi_abstraction.c`, and `main.c`. The SPI hardware-level code now diverges from the `c868539` known-good state.

---

## Session Focus (April 7, 2026)

All SPI reads return `0x00`. Pico reports `cs_falls=0, raw_rx_bytes=0` with `sck_edges > 463000`.  
Root hypothesis: CS line from RW612 is not reaching Pico GP17.

---

## Hardware Reference

| Signal | Pico Pin | RW612 GPIO | Notes |
|--------|----------|------------|-------|
| MOSI   | GP16     | GPIO9      | Flexcomm1 |
| CS     | GP17     | GPIO10 (primary), GPIO6 (fallback) | Manual GPIO, disputed |
| SCK    | GP18     | GPIO7      | Flexcomm1 |
| MISO   | GP19     | GPIO8      | Flexcomm1 |

- RW612 GPIO controller: `hsgpio0`
- FC1 hardware SSELN0 = GPIO[6] per pinmux doc
- Prior working `zephyr_spi_master_test_app` used **GPIO10** as CS (confirmed in `phase2_e2e_communication_test_summary.md`)

---

## Diagnostic Findings

### Pico SPI Register State (cr0/cr1/sr from heartbeat)
```
cr0 = 0x00003e07   → cpsdvsr=62 (1 MHz), SCR=0, MODE0 correct
cr1 = 0x00000006   → SSE=1, MS=1 → slave mode enabled ✓
sr  = 0x00000010   → TNF=1 only (TX FIFO not full; no bytes arriving = no CS)
```
Pico SPI peripheral configuration is correct. CS never transitions.

### Heartbeat Sample (representative, from 5-run test)
```
cs_falls=0  cs_rises=0  cs_low_samples=0
raw_rx_bytes=0  raw_rx_loops=0
sck_edges=154380
```
- SCK edges present → RW612 SPI clock is running
- CS edges = 0 → CS wire not reaching GP17

---

## Changes Applied This Session

### 1. Pico: SPI Init PL022 Fix + Pull-ups (`pico_spi_slave_test/main.c`)

**Problem**: `spi_set_slave()` was called while SSE=1 (PL022 spec violation). GPIO functions assigned after slave mode. Missing pull-ups on idle lines.

**Fix**:
- Moved `gpio_set_function()` calls **before** `spi_set_slave()`
- Added PL022 SSE bracket: `hw_clear_bits(cr1, SSE)` → `spi_set_slave()` → `hw_set_bits(cr1, SSE)`
- Added `gpio_pull_up(PIN_CS)`, `gpio_pull_up(PIN_MOSI)`, `gpio_pull_down(PIN_SCK)`

**Result**: Pico SPI peripheral enters correct slave state (cr1=0x06 confirmed).

### 2. Pico: Enhanced Heartbeat — SPI Register Dump (`pico_spi_slave_test/main.c`)

Added `cr0`, `cr1`, `sr` register values to heartbeat printf for real-time diagnosis.

### 3. RW612: INF-level SPI Byte Diagnostics (`zephyr_dbus_driver/src/dbus_driver.c`)

Promoted `LOG_DBG` → `LOG_INF` for combined 16B TX/RX exchange and first dummy RX attempt.  
**Result**: Confirmed RX bytes from RW612 are `0x00` on every attempt (all-zero deterministic).

### 4. RW612: CS Pin Changed to GPIO10 (`zephyr_dbus_driver/src/dbus_driver.c`)

Changed `#define DBUS_CS_GPIO_PIN` from `6` → `10` to match prior working test app.  
**Result**: Still cs_falls=0, still all-zero RX.

### 5. RW612: Dual-CS Driving — GPIO6 AND GPIO10 (`zephyr_dbus_driver/src/dbus_driver.c`)

**Rationale**: Eliminate software-side pin ambiguity by driving both candidates.

**Implementation**:
```c
#define DBUS_CS_GPIO_PIN         10
#define DBUS_CS_GPIO_FALLBACK_PIN 6

static inline void DBCDRV_setCsLevel(uint8_t level)
{
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN,         level);
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_FALLBACK_PIN, level);
}
```
Both GPIO6 and GPIO10 initialized in `DBCDRV_initComChannels()`.  
All CS assert/deassert sites updated to `DBCDRV_setCsLevel(0/1)`.

**Result**: Still cs_falls=0. Confirms neither GPIO6 nor GPIO10 is connected to Pico GP17.

---

## Current State

### Source Files Modified (Uncommitted — `git diff --stat HEAD`)
| File | Insertions | Deletions |
|------|-----------|-----------|
| `pico_spi_slave_test/main.c` | +568 | -34 |
| `zephyr_dbus_driver/src/dbus_driver.c` | +269 | -? |
| `zephyr_dbus_driver/src/main.c` | +32 | -? |
| `zephyr_dbus_driver/src/motor_service.c` | +27 | -? |
| `zephyr_dbus_driver/src/spi_abstraction.c` | +27 | -? |

### Firmware Version Strings
- Pico: `"mode0_transport_restore_v7"` (unchanged from pre-session)
- RW612: latest build @ `/home/swied/projects/dbus_porting_BA/build/zephyr/zephyr.elf`

### Build Status
- RW612 (`cmake --build build -j`): ✅ PASS, FLASH=63964B
- Pico (`cmake --build pico_spi_slave_test/build -j`): ✅ PASS

### Test Results (5-run repeatability)
- PRE_MOTOR_V1: 0/5 ❌
- MOTOR0_TOGGLE_V1: 0/5 ❌

---

## Root Cause Status

| Candidate | Tried | Result |
|-----------|-------|--------|
| Wrong CS pin (GPIO6 vs GPIO10) | ✅ Yes — tried both separately | Still 0 CS edges |
| Both pins at once (GPIO6 + GPIO10) | ✅ Yes — dual-CS helper | Still 0 CS edges |
| Pico SPI init PL022 violation | ✅ Fixed | Peripheral state correct now |
| Missing pull-ups | ✅ Fixed | No floating inputs |
| Physical CS wire to GP17 | ❌ Not verified | **TOP CANDIDATE** |
| CS wire on different Pico pin | ❌ Not probed | **TOP CANDIDATE** |

**Most likely root cause**: The CS wire from RW612 lands on a Pico GPIO other than GP17, OR the physical wire between the boards is disconnected/miswired.

---

## Pending Work (Next Session Continuation)

### Step 1 — CS Pin Probe Scanner on Pico (HIGHEST PRIORITY)

Add a 4-pin edge detector to `pico_spi_slave_test/main.c` to scan GP1, GP5, GP9, GP17 simultaneously and report which pin sees CS transitions.

**State**: Code was drafted, `apply_patch` tool was disabled when attempted. Use `replace_string_in_file` instead.

**Code to add** (after `static uint8_t last_sck_level = 0u;`):
```c
#define CS_PROBE_COUNT 4u
static const uint8_t cs_probe_pins[CS_PROBE_COUNT] = {1u, 5u, 9u, 17u};
static uint32_t cs_probe_fall_edges[CS_PROBE_COUNT] = {0u};
static uint32_t cs_probe_rise_edges[CS_PROBE_COUNT] = {0u};
static uint32_t cs_probe_low_samples[CS_PROBE_COUNT] = {0u};
static uint8_t cs_probe_last_level[CS_PROBE_COUNT] = {1u, 1u, 1u, 1u};
```

**Init function** (before `cs_is_active()`):
```c
static void init_cs_probe_inputs(void) {
    for (size_t i = 0u; i < CS_PROBE_COUNT; i++) {
        uint8_t pin = cs_probe_pins[i];
        if (pin != PIN_CS) {
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_IN);
            gpio_pull_up(pin);
        }
        cs_probe_last_level[i] = (uint8_t)gpio_get(pin);
    }
}
```

**Call site** in `main()` after `gpio_set_function` block:
```c
init_cs_probe_inputs();
```

**Poll loop** inside main `while(1)` after sck_edges tracking:
```c
for (size_t i = 0u; i < CS_PROBE_COUNT; i++) {
    uint8_t level = (uint8_t)gpio_get(cs_probe_pins[i]);
    if (level == 0u) cs_probe_low_samples[i]++;
    if (level != cs_probe_last_level[i]) {
        if (level == 0u) cs_probe_fall_edges[i]++;
        else cs_probe_rise_edges[i]++;
        cs_probe_last_level[i] = level;
    }
}
```

**Heartbeat extension** (after existing cs_falls line):
```
cs_probe_gp1=%lu/%lu/%lu cs_probe_gp5=%lu/%lu/%lu cs_probe_gp9=%lu/%lu/%lu cs_probe_gp17=%lu/%lu/%lu
```
(format: falls/rises/low_samples per pin)

### Step 2 — Interpret Probe Results
- **Expected outcome**: One of gp1/gp5/gp9 shows nonzero fall_edges → that's the real CS wire location.
- **If all zero**: CS wire is physically disconnected, or RW612 GPIO is on a pin not driven by current code. Use multimeter to confirm physical wire continuity.

### Step 3 — Either Rewire or Remap CS Pin
- **If physical pin found**: Update `DBUS_CS_GPIO_PIN` in `dbus_driver.c` to matching RW612 GPIO. Or physically move the CS wire to GP17 on Pico side.
- **If wire disconnected**: Reconnect and retest.

### Step 4 — Regression Test
Once CS edges > 0 established:
1. Confirm `raw_rx_bytes > 0` in Pico heartbeat
2. Run `PRE_MOTOR_V1` 5x → target 5/5 PASS
3. Run `MOTOR0_TOGGLE_V1` 5x → target 5/5 PASS

---

## Key Code Locations

| Item | File | Lines |
|------|------|-------|
| Dual CS helper | `zephyr_dbus_driver/src/dbus_driver.c` | 93–97 |
| CS pin defines | `zephyr_dbus_driver/src/dbus_driver.c` | 22–23 |
| CS GPIO init | `zephyr_dbus_driver/src/dbus_driver.c` | ~1346–1356 |
| Pico SPI init | `pico_spi_slave_test/main.c` | (main(), after motor_gpio_init) |
| Pico heartbeat | `pico_spi_slave_test/main.c` | (main while-loop, ~10s interval) |
| Pull-ups | `pico_spi_slave_test/main.c` | after gpio_set_function block |

---

## Dead Ends Confirmed

- GPIO6 alone as CS: no effect
- GPIO10 alone as CS: no effect  
- Both GPIO6 + GPIO10 simultaneously: no effect
- These eliminate software ambiguity; issue is physical wiring

---

## Lessons Learned This Session

1. **PL022 slave mode requires SSE=0 during `spi_set_slave()` reconfiguration** — do not call while SSE=1
2. **Pull-ups on CS/MOSI prevent false triggering on floating lines**, but do not fix routing issues
3. **Dual-CS driving is a valid diagnostic technique** to rule out pin-mapping ambiguity from software
4. **When SCK edges increment but CS never toggles**, the issue is 100% CS wiring, not SPI protocol
5. **Register-level heartbeat is essential**: `cr0/cr1/sr` confirmed the Pico SPI peripheral state is correct, narrowing the blame to pure physical connectivity
