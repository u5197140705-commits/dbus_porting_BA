# Session Protocol — 2026-05-19

## Objective
Fix the Pico2 SPI read-response path so RW612 `DBCDRV_readReg32` receives a valid
`A0 50 14 01 ...` frame from the Pico slave without using the RW612-side shadow
fallback. Goal: readback probe `PASS` with real Pico register data, not cached values.

---

## Starting State
- RW612 shadow fallback masking the read failure; functional PASS was fake.
- `DBCDRV_spiTransceiveBytewise` existed by name but internally still used a single
  8-byte `spi_transceive` burst.
- Pico2 TX FIFO consistently returning `40 00 00 00 00 00 00 00` on every dummy read
  attempt — byte 0 correct (`wire[0]` of response), bytes 1-7 zeros.
- Firmware: `MAIN_HWSSEL_V17_2026-05-19`.

---

## Problems Encountered and Root Causes

### Problem 1 — RW612: `spiTransceiveBytewise` was a burst, not bytewise
**Observed symptom**: The function name implied 1-byte granularity but the
implementation used a single `spi_transceive(8 bytes)` call.

**Root cause**: The helper function was a stub that never received the loop treatment.

**Fix (RW612, `dbus_driver.c`)**:  
Replaced the single-transfer body with a `for` loop over 8 individual
`spi_transceive(1 byte)` calls, all inside the same manual CS (GPIO10) assertion
window.

**Outcome**: Bytes began appearing more consistently, but bytes 1-7 were still
zeros (`40 00 00 00 00 00 00 00`). The change confirmed that the transfer granularity
was not the limiting factor.

---

### Problem 2 — Pico: `sniff_process_frame()` calling `spi_slave_rearm()` mid-transfer (V17 → V18)
**Observed symptom**: RW612 consistently received `40 00 00 00 00 00 00 00` on
every dummy read attempt. Pico heartbeat showed `tx=8` (FIFO loaded), but bytes
1-7 were always zero/idle.

**Root cause**:  
`sniff_update()` runs in the Pico main loop on every iteration. During a dummy read
exchange, `sniff_update()` samples SCK/MOSI via GPIO bit-banging, reassembles the
dummy frame (`40 50 14 01 00 00 00 00`), identifies it as a READ command, and calls
`sniff_process_frame()`. That function (pre-V18) called `spi_slave_rearm()` and
`spi_slave_queue_current_tx_frame()`.

At 10 kHz SPI each byte takes 800 µs. The Pico main loop at 125 MHz runs thousands
of iterations per byte. `sniff_process_frame()` therefore fired **between byte 0 and
byte 1** of the active dummy transfer, calling `spi_slave_rearm()` which did a full
`spi_deinit() + spi_init()`. This reset the hardware SSP peripheral and flushed the
TX FIFO. Byte 0 (`0x40`) had already been shifted out. Bytes 1-7 underflowed to
`0x00` because the FIFO was empty.

**Fix (Pico, `main.c`, V18)**:  
Removed `spi_slave_rearm()` and `spi_slave_queue_current_tx_frame()` from
`sniff_process_frame()` READ branch. The sniff path may still update
`tx_frame_desired` / `tx_frame_wire` (read-only data preparation), but must never
touch the hardware SSP FIFO or reset the peripheral.

**Outcome**: `tx=1` appeared (worse than expected), and bytes 1-7 changed pattern.
A new root cause was uncovered.

---

### Problem 3 — Pico: `handle_rx_byte()` calling `process_rx_frame()` mid-transfer — twice per byte (V18 → V19)
**Observed symptom**: Pico heartbeat `tx=1` (only 1 byte loaded into TX FIFO after
queue). `f8sok=0` (streaming decode path never succeeds). All dummy reads return
`40 00 00 00 00 00 00 00`.

**Root cause**:  
`handle_rx_byte()` was accumulating received bytes AND calling `process_rx_frame()`
at two internal triggers:

1. **8-byte boundary check** (inline): when `rx_index == FRAME_SIZE`, it called
   `process_rx_frame()` and reset `rx_index = 0`. This fired mid-transfer (CS still
   active, bottom service loop running), calling `spi_slave_rearm()` while the SSP
   was in active use.

2. **Streaming window decode** (every byte): after the first 8 bytes, the streaming
   window path ran `process_rx_frame()` on **every single incoming byte** via a
   sliding 8-byte window. Each call triggered `spi_slave_rearm()`. For a 16-attempt
   dummy exchange loop (16 × 8 bytes = 128 bytes), this caused 120+ rearm calls
   mid-transfer.

The streaming calls also called `process_rx_frame()` on the `rx_stream_window` copy,
which meant each call could land on a READ frame pattern (the dummy TX is a valid
READ cmd) and trigger `spi_slave_rearm()` again before the previous FIFO load was
finished, leaving only 1 slot free → `tx=1`.

Additionally: `rx_index = 0` reset inside `handle_rx_byte` meant the cs_end handler
in `service_spi_frame()` always saw `rx_index = 0`, so its own `process_rx_frame()`
call (the safe, post-CS path) was never reached.

**Fix (Pico, `main.c`, V19)**:  
- Removed the 8-byte boundary `process_rx_frame()` call and `rx_index = 0` reset
  from `handle_rx_byte()`. The function now only accumulates bytes.
- Removed the streaming window `process_rx_frame()` call entirely. Window
  accumulation is kept for future diagnostics but no decode is triggered.
- The existing cs_end handler in `service_spi_frame()` already called
  `process_rx_frame()` when `rx_index == FRAME_SIZE`. With the inline resets removed,
  `rx_index` reaches 8 naturally and cs_end processes the complete frame safely after
  CS deasserts.

**Outcome**: `f8ok = f8` (100% sniff decode rate). `lf8_cmd=0x40` and
`lf8_val=0x00000456` confirmed sniff path correctly reads the write. Yet `tx=1`
and `lflen=1` persisted. A third root cause was uncovered.

---

### Problem 4 — Pico: `spi_slave_rearm()` using `spi_deinit()+spi_init()` briefly enters master mode, causing spurious cs_end events (V19 → V20)
**Observed symptom**: `tx=1`, `lflen=1`, all reads still `40 00 00 00 00 00 00 00`.
V19 write RX showed `4b 4b 4b 4b 4b 4b 4b 4b` (default pattern) — indicating FIFO
default state correctly, but then `40 40 40 40 40 40 40 40` (wire[0] repeated) on
read dummies.

**Root cause**:  
`spi_slave_rearm()` called `spi_deinit()` (which asserts peripheral reset) and then
`spi_init()`. The Pico SDK `spi_init()` enables the SSP in **master mode** (MS=0)
with SSE=1. In master mode on RP2040/RP2350, GP17 (SSEL) is driven as a **GPIO
output** by the SSP hardware — specifically, the SSP drives it HIGH (SSEL deasserted
in master mode = active-low CS driven inactive).

`cs_poll_update()` calls `gpio_get(PIN_CS)` (= `gpio_get(GP17)`) on every main loop
iteration to detect CS transitions. When `spi_init()` briefly drives GP17 HIGH as
master SSEL output, `gpio_get(GP17)` reads HIGH (CS inactive) even though RW612's
GPIO10 line may be or may have just been LOW. `cs_poll_update()` records this as a
**spurious cs_end** event: `cs_end_pending = true`.

The spurious `cs_end_pending` fires `service_spi_frame()`'s cs_end handler. The drain
loop runs and finds either 0 or 1 byte in the RX FIFO (because only 1 SCK-edge's
worth of data had arrived). Result:
- `rx_index = 1` → partial frame path → `frame_other_total++`, `rx_index = 0`
- `process_rx_frame()` NOT called
- `tx_frame_prequeued` NOT set

The real CS deassert later fires another cs_end, but now `rx_index = 0` (already
reset by the spurious event). The drain loop finds the remaining 7 bytes and
`rx_index = 7`, again a partial frame. `process_rx_frame()` is skipped again.

`spi_set_slave(spi, true)` is also called AFTER `spi_init()` (which sets SSE=1 in
master mode). Per the ARM PL022 spec, the MS bit must only be written when SSE=0.
Setting MS=1 while SSE=1 is undefined behaviour and may not properly transition to
slave mode.

**Fix (Pico, `main.c`, V20)**:  
Replaced `spi_deinit() + spi_init() + spi_set_format() + spi_set_slave()` with a
minimal SSE-bit toggle:
```c
hw_clear_bits(&hw->cr1, SPI_SSPCR1_SSE_BITS);  /* SSE=0: flush FIFOs */
hw_set_bits(&hw->cr1, SPI_SSPCR1_SSE_BITS);    /* SSE=1: re-enable   */
```
Per PL022 spec, clearing SSE resets both TX and RX FIFOs without changing baudrate,
format, or master/slave configuration. The SSP remains in slave mode throughout,
GP17 is never driven as an output, and no spurious CS events are generated.

**Outcome**: V20 flashed — still under evaluation at end of session. `tx=1` and
`lflen=1` still present. Root cause of remaining issue TBD.

---

### Problem 5 — V20: `tx=1` and `lflen=1` persist after SSE-toggle rearm (open)
**Observed symptom**: Same pattern as V19 — `40 00 00 00 00 00 00 00` on all dummy
reads. `tx=1` in heartbeat. `lflen=1` (last frame = 1 byte = partial frame).

**Hypothesis at session end**:  
The SSE-toggle cleared the GP17-glitch path, but partial frame detections (`lflen=1`)
continue. The cs_end handler still doesn't see `rx_index == 8` at the right moment.
Possible remaining causes:
- The bottom service loop in `service_spi_frame()` drains the RX FIFO byte-by-byte
  during the transfer. By the time cs_end fires, the RX FIFO may be empty and
  `rx_index` was accumulated to 8 in the bottom loop — but the cs_end drain loop
  finds nothing and falls through `rx_index=0` path (wait: rx_index should be 8 at
  that point now that we don't reset it in handle_rx_byte — needs re-verification).
- The bytewise transfers on RW612 (8 separate `spi_transceive(1)` calls within one
  GPIO10 CS window) may create brief inter-byte gaps on SCK that confuse the Pico
  SSP's FIFO timing at low SPI frequency (10 kHz).
- The cs_end drain loop timeout parameters (`CS_END_DRAIN_IDLE_US`,
  `CS_END_DRAIN_TIMEOUT_US`) may be too short, causing premature exit before all 8
  bytes settle in the RX FIFO.

**Status**: Open at end of session.

---

## Architectural Learnings

| # | Learning |
|---|----------|
| 1 | `spi_slave_rearm()` must NEVER be called while CS (GP17) is active — any code path that calls it from the main loop (sniff, handle_rx_byte) does so mid-transfer |
| 2 | The correct single safe call site for rearm + FIFO load is the cs_end handler in `service_spi_frame()`, after `cs_end_pending` has been set (CS is guaranteed deasserted) |
| 3 | `handle_rx_byte()` is called from the bottom service loop which runs continuously including during active transfers — it must NEVER call `process_rx_frame()` or `spi_slave_rearm()` |
| 4 | The streaming window in `handle_rx_byte()` fires `process_rx_frame()` once per received byte (after the first 8), not once per frame — this is orders of magnitude more calls than expected and was the cause of the `tx=1` underflow |
| 5 | The inline `rx_index = 0` reset inside `handle_rx_byte()`'s 8-byte boundary check caused cs_end to always see `rx_index = 0`, silently preventing it from calling `process_rx_frame()` |
| 6 | `spi_deinit()+spi_init()` on RP2040/RP2350 briefly puts the SSP into master mode where GP17 is driven as SSEL output — `gpio_get(GP17)` reads this as CS-inactive and fires spurious cs_end events |
| 7 | `spi_set_slave(true)` must only be called when SSE=0 per the ARM PL022 spec; calling it while SSE=1 (as `spi_init()` leaves it) is undefined and may not actually set slave mode |
| 8 | The correct FIFO flush for PL022 in slave mode: toggle SSE (clear then set bit 1 of CR1) — stays in slave mode, no GPIO direction changes, no baudrate reconfiguration |
| 9 | `tx` in heartbeat = `tx_index` after the last queue operation; `tx=1` means the FIFO had 7 bytes already when the queue ran and only 1 slot was free |
| 10 | `lflen=N` = bytes counted at last cs_end event; `lflen=1` means a partial (1-byte) frame was detected → `process_rx_frame()` was skipped |
| 11 | Wire transform: `wire[i] = (desired[i]<<1)|(desired[i+1]>>7)` — first byte of `A0 50 14 01...` response becomes `wire[0]=0x40`; consistent `0x40` in byte 0 of read dummy RX confirms Pico knows the correct response and loads at least byte 0 |
| 12 | The RW612 bytewise `spiTransceiveBytewise` (8 × 1-byte `spi_transceive`) keeps GPIO10 (manual CS) low throughout all 8 sub-transfers — secondary Pico sees a normal continuous 8-byte frame from its SSEL perspective |
| 13 | `f8ok=f8` (sniff decode rate 100%) but `f8sok=0` (stream decode 0%) is now the expected state after removing stream decode; both are correct post-V19 |
| 14 | `sniff_process_frame()` can update `tx_frame_desired`/`tx_frame_wire` (data preparation) at any time safely; it must NOT call `spi_slave_rearm()` or `spi_slave_queue_current_tx_frame()` |

---

## Firmware Version History (Today)

| Version | Key Change | Result |
|---------|-----------|--------|
| V17 | Baseline — sniff path had rearm+queue, bytewise transfers enabled | `40 00 00 00 00 00 00 00` |
| V18 | Removed `spi_slave_rearm` from `sniff_process_frame()` | `tx=1`, streaming window problem exposed |
| V19 | Removed all `process_rx_frame()` calls from `handle_rx_byte()` | `4b 4b 4b 4b...` on writes, partial frames `lflen=1` |
| V20 | Replaced `spi_deinit+spi_init` rearm with SSE-toggle | Partial frames persist — open |

---

## File Change Summary

| File | Change |
|------|--------|
| `zephyr_dbus_driver/src/dbus_driver.c` | `DBCDRV_spiTransceiveBytewise`: single burst → 8 × 1-byte `spi_transceive` loop |
| `pico_spi_slave_test/main.c` | V18: removed `spi_slave_rearm` + queue from `sniff_process_frame()` READ branch |
| `pico_spi_slave_test/main.c` | V19: removed `process_rx_frame()` calls from `handle_rx_byte()` (both boundary and streaming) |
| `pico_spi_slave_test/main.c` | V20: replaced `spi_slave_rearm()` body with SSE-bit toggle |

---

## Current State at Session End

- **Pico2 firmware**: `MAIN_HWSSEL_V20_2026-05-19` (`dbal_motor_v1_onehot_v3_isoD_2026-05-19`)
- **RW612 firmware**: `AUTO_TEST_ON_QUAD_SIMULTANEOUS_2026_05_13` (unchanged today)
- **Read response**: Still failing — `40 00 00 00 00 00 00 00` on all dummy attempts
- **Write path**: Working correctly (motor enable `mA_en=1` confirmed, values stored correctly `lf8_val=0x00000456`)
- **Sniff decode**: 100% correct (`f8ok=f8`, `lf8_addr=0x5014 lf8_val=0x00000456`)
- **Readback probe**: FAIL — `err=1`, `got=0x00000000` vs `expected=0x00000456`

## Immediate Next Debug Steps

1. Add explicit diagnostic `printf` inside `process_rx_frame()` READ branch (cs_end path) to confirm it IS or IS NOT being reached in V20.
2. Check whether `rx_index` actually reaches 8 in the cs_end drain loop for the CMD exchange — add `lflen` logging to differentiate between "cs_end ran but rx_index=0" vs "cs_end ran and rx_index=8 but process_rx_frame failed".
3. Verify `CS_END_DRAIN_IDLE_US` and `CS_END_DRAIN_TIMEOUT_US` are large enough for the 10 kHz SPI rate (each byte = 800 µs).
4. Consider disabling `sniff_update()` entirely as a diagnostic step to eliminate any remaining sniff→CS interaction.
5. If process_rx_frame IS reached, add a print of `tx_index` immediately after `spi_slave_queue_current_tx_frame()` to confirm all 8 bytes loaded.
