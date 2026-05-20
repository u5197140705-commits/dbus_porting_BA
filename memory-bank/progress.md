# Project Progress

**Task: Zephyr DBus Driver Project - CAN to SPI Migration**
*   **Status:** Completed
*   **Date Completed:** 2025-08-28
*   **Summary:** The project successfully migrated from a non-existent CAN communication layer to an SPI-based communication layer. This involved significant code refactoring, device tree overlay creation, and Kconfig updates. A detailed protocol of the process, including encountered errors and solutions, has been generated and stored in `memory-bank/protocol_can_to_spi_migration.md`.

**Next Major Milestone:** (To be defined based on future tasks)

## Task: RW612 Four-Motor Validation
*   **Status:** Completed
*   **Date Updated:** 2026-05-20
*   **Summary:** A dedicated RW612 validation image (`AUTO_TEST_ALL4_V1_2026_05_20`) was built and exercised against all four wired motors. The harness switched out of secondary-only readback mode, enabled the auto motor test, and ran three simultaneous rounds across motors 0..3 with target switching between Pico1 and Pico2. The bench log completed with `PASS: 1`, `FAIL: 0`, and a clean final stop sequence.

### Verified Result
*   The correct quad-motor image booted: `Hello from Zephyr DBus Driver project! [AUTO_TEST_ALL4_V1]` and `Main: mode=AUTO_TEST_ALL4_V1`.
*   Boot-time pre-disable reached both targets: primary writes hit `0x5000` and `0x5020`, while secondary writes hit `0x5010` and `0x5030`.
*   The simultaneous test ran all three rounds successfully with the intended speed sets:
*   Round 1: `m0=200`, `m1=500`, `m2=800`, `m3=1100`
*   Round 2: `m0=650`, `m1=1000`, `m2=300`, `m3=750`
*   Round 3: `m0=1100`, `m1=250`, `m2=900`, `m3=450`
*   The run completed with `Main: RUN 1 RESULT: PASS`, summary `PASS: 1`, `FAIL: 0`, `Overall: PASS`, and a final stop burst to all four enable registers.

### Scope Of What Is Solved
*   RW612 can now drive and coordinate all four logical motors across both Pico targets in the current direct-register harness.
*   This validates the combined write-path routing for motor0 and motor2 on Pico1 plus motor1 and motor3 on Pico2 under a real multi-node exercise, not just isolated single-target probes.
*   The next major functional step is endstop integration and interrupt-driven stop handling, not further basic four-motor bring-up.

## Task: DBus Driver Structure Analysis
*   **Status:** Completed
*   **Date Completed:** 2025-09-12 (Current Date)
*   **Summary:** A comprehensive analysis of the original DBus driver's file structure, architectural layers, component relationships, and external dependencies was performed. A detailed protocol of this analysis, including a Mermaid diagram, has been generated and stored in `dbus_porting_protocol.md`. This analysis provides a foundational understanding for the upcoming porting effort.

## Task: RW612 <-> Pico2 SPI Readback Debugging
*   **Status:** Completed
*   **Date Updated:** 2026-05-20
*   **Summary:** Extensive transport-level debugging was performed on the RW612 secondary SPI path and the Pico2 SPI slave test firmware to restore register readback for motor register `0x5014`. Bench validation now succeeds: RW612 writes `0x00000456`, reconstructs the fragmented secondary response stream, and reads back `0x00000456` correctly.

### Key Learnings
*   Flashing mistakes can invalidate transport conclusions. The active RW612 image is `zephyr_dbus_driver/build_local/zephyr/zephyr.elf` with marker `SECONDARY_READBACK_V1_2026_05_19`; older root `build/zephyr` artifacts were stale.
*   RW612 secondary manual CS must be initialized before any target selection. `DBCDRV_setSpiTarget()` can run before normal init, so GPIO10 has to be self-configured idle-high in `dbus_driver.c`.
*   On Pico, the GPIO sniffer path is more trustworthy than PL022 RX diagnostics for command recognition. Heartbeat fields like `lflen` and `foth` often look poor even while `sniff_process_frame()` correctly identifies READ/WRITE commands.
*   The Pico-side read value generation is not the primary failure anymore. Multiple runs showed the Pico correctly tracking writes locally, recognizing READs to `0x5014`, and persistently preparing the exact response `A0 50 14 01 56 04 00 00`.
*   RW612 continuous 16-byte read probing was not compatible with the Pico’s fixed 8-byte sniffer model. Removing the continuous probe simplified interpretation and ruled it out as the controlling issue.
*   CS timing and lifecycle matter on Pico. Important fixes included reducing `CS_END_DRAIN_IDLE_US`, servicing `cs_end_pending` before `cs_start_pending`, and avoiding mid-transaction FIFO flush/reset behavior.
*   `tx_index` / preload state is a critical diagnostic. A heartbeat with `tx=0` after traffic indicated the next response frame had not actually been queued. Later direct-DR preload experiments showed `tx=8`, proving the queue path can stage bytes even though RW612 still receives fragments.
*   Live `tx=` snapshots are too timing-sensitive to diagnose the handoff on their own. New latched Pico counters `qend`, `qstart`, `txfall`, `txrise`, and `txact` were required to distinguish idle preload, CS-start queueing, and active-transfer top-off behavior.
*   Those latched counters materially changed the diagnosis. The writable-gated queue path often accepted only one byte on the real transaction (`qstart=1`), while a forced CS-start direct-DR burst proved the Pico can queue all 8 bytes at the decisive moment (`qstart=8`).
*   Raw RX signatures are informative:
*   `0x4B` on RW612 corresponds to transformed Pico filler and indicates stale/default response data.
*   Later patterns containing isolated `0x01`, `0x40`, `0x50`, `0x14`, or `0xA0` show pieces of the intended response escaping across retries rather than a fully formed 8-byte frame arriving at once.
*   The decisive May 20 fix was on RW612 fallback quality, not another Pico transport rewrite. Keeping the best provisional interleaved payload by payload quality prevented late filler-heavy candidates like `ff ff ff fe` from overwriting the first correct `56 04 00 00` reconstruction.
*   RW612 read recovery was extended to seed cumulative reconstruction with the initial command-exchange RX instead of discarding it completely, because recent traces show the response may begin in that first exchange and continue across later retries.

### Current Best Known State
*   RW612 writes to secondary Pico work.
*   Pico heartbeat confirms valid frame recognition and local motor state updates.
*   Pico now proves both response preparation and CS-start force-queueing of the expected `A0 50 14 01 56 04 00 00` frame.
*   RW612 secondary readback now succeeds even though the response is still recovered from fragmented retry traffic rather than one contiguous parsed frame.
*   Latest bench validation: provisional interleaved matches stabilize at `payload=56 04 00 00`, RW612 logs `using best provisional interleaved payload score=4`, and the summary returns `value=0x00000456` for address `0x5014` on the secondary target.

### Verified Result
*   Earlier failing runs still showed the transport shape clearly: repeated lane-0 `0xA0` bytes and fail summaries with no usable payload beyond the response marker.
*   After the scored-candidate fix, the secondary probe improved from false-positive `0xfeffffff` recovery to stable correct reconstruction beginning at attempt 16.
*   Bench validation succeeded: `Main: readback probe result motor=1 expected=0x00000456 got=0x00000456 err=0`.

### Scope Of What Is Solved
*   Pico2 / secondary register readback for the current probe case is now functionally restored.
*   The solution still depends on RW612-side cumulative/interleaved reconstruction of fragmented retry data; it should not yet be described as receiving a clean contiguous native response frame from Pico2.
*   Future follow-up, if needed, should focus on improving secondary transport cleanliness or reducing dependence on fallback reconstruction, not on reopening the already-validated register storage or address-selection path.

## Task: RW612 <-> Pico1 Native Readback Recovery
*   **Status:** Completed
*   **Date Updated:** 2026-05-20
*   **Summary:** The Pico1 readback probe is now working end-to-end. After switching the probe to Pico1 and preserving Pico-side read response progress across retries, RW612 successfully reconstructed the sparse one-byte-per-retry response stream and read back `0x00000456` from register `0x5004`.

### Verified Result
*   Pico heartbeat now shows the prepared response is correct: `lrrd=a050040156040000` and `lrrv=0x00000456`.
*   RW612 read retries no longer need a contiguous response frame in one transaction. The successful wire pattern was observed as ordered first-byte fragments across retries: `a0`, `50`, `04`, `01`, `56`, `04`, then zeros.
*   `DBCDRV_readReg32()` now has a sparse reconstruction path that scans the first byte of each retry frame and matches the interleaved response header/payload.
*   Bench validation succeeded: `Main: readback probe result motor=0 expected=0x00000456 got=0x00000456 err=0`.

### Scope Of What Is Solved
*   The remaining readback bug is no longer on the shared RW612 reconstruction path for this Pico1 probe case.
*   This checkpoint proves the transport can complete native readback even when the slave releases only one meaningful byte per retry transaction.
*   The later Pico2 / secondary-path validation is now also complete, so the broader multi-node readback checkpoint has been reached for the current probe cases.