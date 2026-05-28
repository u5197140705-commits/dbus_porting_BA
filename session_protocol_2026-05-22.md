# Session Protocol — 2026-05-22

## Objective
Stabilize integrated SPI readback with both Pico slaves powered at the same time while keeping the normal RW612 integrated image and the motor test flow intact.

---

## Current State
- The original integrated RW612 image has now been re-run successfully.
- In that integrated run, motor control and the quad test both passed.
- Integrated readback now splits by target:
  - primary readback succeeded with `expected=0x00000123 got=0x00000123`
  - secondary readback failed with `expected=0x00000456 got=0x00000000`
- This means the problem is no longer a generic "combined bus readback is broken" issue. The combined bus can return correct read data for the primary Pico while both Picos are powered.

---

## What Is Already Proven
- Current-tree primary-only RW612 isolation readback passed earlier in the session.
- The original integrated RW612 image can run the quad motor test flow successfully with both Picos powered.
- The original integrated RW612 image can also read back motor 0 correctly from the primary Pico with both Picos powered.
- Secondary readback is still failing in current bench state, returning `0x00000000` for motor 1.
- Therefore:
  - the shared bus is not completely broken in the combined configuration
  - the MISO-release change likely improved or removed the primary-side combined failure
  - the remaining problem is now concentrated on the secondary read path or on how the inactive/active interaction behaves when the secondary Pico is selected

---

## Most Important Code Change This Session
### Pico inactive-MISO tri-state fix
File: `pico_spi_slave_test/main.c`

Changed `spi_slave_set_miso_active(bool active)` so the unselected Pico releases the shared MISO line instead of continuing to drive it.

Current behavior:
- selected slave: MISO is restored to `GPIO_FUNC_SPI` with normal output-enable behavior
- inactive slave: MISO is switched to plain GPIO input with pulls disabled for a stronger high-impedance state

Reason:
- with two powered RP2040 slaves on one shared MISO line, the inactive slave must truly stop driving the line
- otherwise the active slave's response is corrupted or suppressed

Current evaluation:
- this stronger inactive-MISO change did not fix the integrated failure case by itself
- after flashing both Picos with the updated firmware, integrated primary readback still succeeded while integrated secondary readback still returned `0x00000000`
- therefore the remaining interference is not explained by a simple "inactive Pico still drives MISO" model alone

---

## Build Status
### Pico firmware
Rebuilt successfully after the MISO fix:
- `pico_spi_slave_test/build/pico_spi_slave_test.uf2`
- `pico_spi_slave_test/build/pico_spi_slave_test_pico2.uf2`

Firmware markers expected in this build lineage:
- `FW_ID_MAIN "MAIN_HWSSEL_V22_2026-05-22"`
- `PICO_FIRMWARE_VERSION "dbal_motor_v1_onehot_v3_isoD_v22_2026-05-22"`

### RW612 firmware
- The normal integrated validation target remains the standard integrated image, not the isolation images.
- Current integrated source identity in the tree:
  - `AUTO_TEST_ALL4_V1`
- Isolation images already exist and were only used to prove each Pico path independently.

---

## Important Bench Conclusions
1. Repeated iterations happened mainly because several candidate causes could all plausibly explain missing readback on the bench:
   - wrong flashed RW612 image
   - transport timing differences
   - CS behavior differences
   - Pico-side response preparation timing
   - shared-bus electrical contention
2. The decisive reduction only came from isolation testing.
3. The new integrated run is the strongest evidence so far because it shows the original image, both Picos powered, quad motor test passing, and primary readback succeeding.
4. Because primary readback works in the combined configuration, the current blocker is narrower than generic bus contention.
5. The remaining failure is specific to the secondary read path: either secondary response generation, secondary selection timing, or interference that only appears when the secondary Pico is the selected slave.

---

## Files Most Relevant Right Now
- `pico_spi_slave_test/main.c`
- `zephyr_dbus_driver/src/dbus_driver.c`
- `zephyr_dbus_driver/src/main.c`
- `zephyr_dbus_driver/boards/frdm_rw612/frdm_rw612.overlay`
- `project_pinmap.md`
- `readback_debugging_postmortem.md`

---

## Current Hardware / Bus State To Remember
- RW612 is SPI master on Flexcomm1.
- Shared bus lines:
  - RW612 GPIO7 -> Pico GP18 SCK
  - RW612 GPIO9 -> Pico GP16 MOSI
  - RW612 GPIO8 <- Pico GP19 MISO
- Primary Pico CS is on RW612 GPIO6 via hardware `SSEL0`.
- Secondary Pico CS is on RW612 GPIO10 as manual GPIO CS.
- Primary Pico owns motors 0 and 2.
- Secondary Pico owns motors 1 and 3.

---

## Latest Bench Update
- A correct run of the original integrated RW612 image produced:
  - quad motor test `PASS`
  - primary readback success: `addr=0x5004 target=primary result=ok value=0x00000123`
  - secondary readback failure: `addr=0x5014 target=secondary result=ok value=0x00000000`
- The RW612 readback helper accepted provisional interleaved data in both cases:
  - primary provisional payload stabilized at `23 01 00 00` and reconstructed the correct value
  - secondary provisional payload stabilized at `00 00 00 00` and reconstructed zero
- This means the current issue is not that readback never works in the integrated image. It works for primary and fails for secondary.
- Important nuance: this is still a transport-level failure masked as success because `DBCDRV_readReg32()` currently accepts a best provisional interleaved payload and can report `result=ok` even when the payload is all zeros.

## Post-Fix Retest Result
- Both Picos were manually reflashed with the newer firmware that adds:
  - inactive MISO switched to GPIO input with pulls disabled
  - USB `BOOTSEL` command support for future reflashing
- Primary Pico BOOTSEL-free reboot command used successfully from this workspace:
  - `powershell.exe -NoProfile -Command '$port = [System.IO.Ports.SerialPort]::new("COM10",115200); $port.NewLine = "`r`n"; $port.DtrEnable = $true; $port.RtsEnable = $true; $port.Open(); Start-Sleep -Milliseconds 200; $port.WriteLine("BOOTSEL"); $port.BaseStream.Flush(); $port.Close(); Write-Output "BOOTSEL sent to COM10"'`
  - after running that command, copy `pico_spi_slave_test/build/pico_spi_slave_test.uf2` to the `RPI-RP2` drive
- After that reflash, the original integrated RW612 image was rerun.
- Result:
  - quad motor test still `PASS`
  - primary readback still `PASS`
  - secondary readback still `FAIL` with provisional payload `00 00 00 00`
- Conclusion:
  - the latest firmware update did not resolve the combined secondary-selected readback failure
  - the remaining problem is likely not just MISO output-enable state
  - the next best discriminator is a hardware-line isolation check with Pico1 still powered

## Physical MISO Isolation Result
- A follow-up hardware isolation test was performed with:
  - Pico1 still powered
  - only Pico1 GP19 (its MISO connection) disconnected from the shared bus
  - all other Pico1 connections left in place
- Result:
  - primary readback failed, which is expected because Pico1 MISO was physically removed from the bus
  - secondary readback succeeded with `expected=0x00000456 got=0x00000456 err=0`
  - secondary provisional payload stabilized at `56 04 00 00`, reconstructing correctly
- This is the decisive discriminator.
- It means Pico1's physical GP19/MISO connection is the specific source of the remaining combined failure.
- The problem is no longer just "Pico1 is present" in a broad sense; it is specifically that Pico1's MISO path still loads or disturbs the shared bus when Pico2 is selected.

## Series-Resistor Retest Result
- Pico1 GP19 was reconnected and the shared MISO bus was rewired with one series resistor in each Pico MISO branch before the merge point.
- The original integrated RW612 image was rerun with both Picos powered.
- Result:
  - quad motor test still `PASS`
  - primary readback still `PASS`
  - secondary readback still `FAIL`, returning `0x00000000`
- This means the simple passive series-isolation change was not sufficient to restore reliable secondary-selected readback in the combined setup.
- The failure remains specific to Pico1's MISO electrical path, but the latest result raises the bar: passive damping alone is not enough.

## New Discriminating Bench Result
- A follow-up run was performed with Pico1 disconnected from `VSYS`, so only Pico2 was powered.
- In that state:
  - primary readback failed with `result=error` and an all-zero fail tail, which is expected because the primary slave was unpowered
  - secondary readback succeeded with `expected=0x00000456 got=0x00000456 err=0`
  - the provisional interleaved payload stabilized at `56 04 00 00`, which reconstructs correctly to `0x00000456`
- This re-establishes that the secondary Pico read-response path itself works when Pico1 is not present as a powered participant on the bus.
- Therefore the remaining problem is narrower: Pico1 being powered and connected is what breaks secondary-selected readback in the combined setup.

## Primary Pico Readback Recovery Checkpoint
- After reflashing the primary Pico with the updated firmware, RW612 was switched to a short primary-only register log mode.
- That short mode did not drive motors 1 and 3; it only exercised register writes and reads for primary motors 0 and 2.
- Result:
  - motor0 speed write/readback passed with `wrote_speed=0x00000123 read_speed=0x00000123`
  - motor2 speed write/readback passed with `wrote_speed=0x00000789 read_speed=0x00000789`
- This confirms the primary Pico readback path is restored for the focused register probe after the latest Pico flash.
- Because the short mode did not keep all four motors moving, a follow-up RW612 mode was added next to test primary readback while all four motors are commanded on.

## Live Quad Primary Readback Result
- The follow-up RW612 live-quad mode was run with all 4 motors commanded on before the primary readback probe.
- Bench observation: all 4 motors did move during this run.
- While those motors were still running, the primary Pico readback stayed correct:
  - motor0 enable readback passed with `read_enable=0x00000001`
  - motor0 speed readback passed with `expected_speed=0x00000123 read_speed=0x00000123`
  - motor2 enable readback passed with `read_enable=0x00000001`
  - motor2 speed readback passed with `expected_speed=0x00000789 read_speed=0x00000789`
- This is the first validated result showing that primary Pico register readback still works while all four motors are actively moving.

## Live Quad Secondary Readback Result
- The analogous RW612 live-quad mode was then run for the secondary Pico while all 4 motors were again commanded on.
- Bench observation: all 4 motors did move during this run as well.
- Secondary enable readback remained correct for both probed motors:
  - motor1 enable readback passed with `read_enable=0x00000001`
  - motor3 enable readback passed with `read_enable=0x00000001`
- Secondary speed readback was mixed:
  - motor1 speed readback failed with `expected_speed=0x000001f4 read_speed=0x000004f4`
  - motor3 speed readback passed with `expected_speed=0x0000044c read_speed=0x0000044c`
- The same live-quad secondary run was repeated once more and reproduced the exact same motor1 mismatch: `expected_speed=0x000001f4 read_speed=0x000004f4`, while motor3 still read back correctly.
- After instrumenting the probe to perform an immediate second speed read for motor1, a follow-up run changed the result again:
  - motor1 first speed readback passed with `read_speed=0x000001f4`
  - motor1 second immediate speed readback also passed with `read_speed_second=0x000001f4`
  - motor3 speed readback still passed with `expected_speed=0x0000044c read_speed=0x0000044c`
- The provisional payload for motor1 briefly showed `f4 00 00 00` before stabilizing to `f4 01 00 00`, which then reconstructed to the correct `0x000001f4`.
- This changes the diagnosis: the secondary live-quad issue is timing-sensitive and probe-sequence-sensitive, not a fixed stable wrong value. The earlier `0x000004f4` result is real, but it is now shown to be perturbable by the read timing.
- A follow-up timing sweep then tested motor1 speed reads at `0 us`, `100 us`, and `500 us` after the first read.
- That result falsified the simple settle-delay hypothesis:
  - motor1 returned the same wrong value on all three reads: `delay0=0x000000f4`, `delay100=0x000000f4`, `delay500=0x000000f4`
  - motor3 also failed in that same run, reading `0x0000104c` instead of `0x0000044c`
- The corruption pattern therefore moved again:
  - earlier runs produced `0x000004f4` for motor1
  - the immediate second-read experiment produced correct `0x000001f4`
  - the delay-sweep experiment produced stable `0x000000f4` for motor1 and `0x0000104c` for motor3
- This now points away from a pure settle-time problem and more toward byte-lane contamination or interleaved payload reconstruction instability under the secondary live-quad load.
- A final discriminating run then separated secondary start order from probe order:
  - secondary start order was kept at the original `motor1 -> motor3`
  - secondary probe order was changed to `motor3 -> motor1`
- Result:
  - motor3 became the failing probe, reading `expected_speed=0x0000044c read_speed=0x00001048`
  - motor1 then read back correctly on all three delay samples: `delay0=0x000001f4`, `delay100=0x000001f4`, `delay500=0x000001f4`
- This is the clearest result so far that the corruption follows probe slot / probe sequencing more than it follows one fixed motor register.
- Current best diagnosis: the secondary live-quad failure is dominated by readback sequencing or payload reconstruction instability on the shared bus, not by one permanently bad secondary motor register.
- A final follow-up added a throwaway warm-up speed read before each logged secondary speed read.
- Two immediate outcomes were observed with that build:
  - one run still showed first-slot corruption on motor3 before the warm-up instrumentation was reflected in the final log line, with `motor3 read_speed=0x00001048`
  - the next run showed both warm-up and logged speed reads cleanly for both motors:
    - motor3 `warmup=0x0000044c read_speed=0x0000044c`
    - motor1 `warmup=0x000001f4 delay0=0x000001f4 delay100=0x000001f4 delay500=0x000001f4`
- This is the strongest evidence so far that the first secondary speed read after probe selection is the unstable one, and that consuming one read before the logged read can stabilize the subsequent payload reconstruction.

## Combined All-4 Readback Result
- A combined mode was then added to start all 4 motors and probe both Picos in one run.
- That merged test did not pass cleanly.
- Primary-side result in the same run was mixed:
  - motor0 passed with `expected_speed=0x00000123 read_speed=0x00000123`
  - motor2 failed with `expected_speed=0x00000789 read_speed=0x00001c89`
- Secondary-side result then degraded further after the primary probe:
  - motor3 enable readback came back as `0x00000000`
  - motor3 warm-up speed read failed with `err=1`
  - motor1 enable read failed with `err=1`
- This means the currently merged “probe primary then secondary in one runtime” sequence is not yet a working all-4 readback test, even though the separated primary-only and secondary-only live-quad modes each produced successful runs on their own.
- A decisive isolation rerun was then performed with Pico1 MISO physically disconnected.
- Result in that state:
  - primary readback failed immediately, which is expected once Pico1 can no longer drive MISO
  - secondary readback completed cleanly in the same merged all-4 mode:
    - motor3 `warmup=0x0000044c read_speed=0x0000044c read_enable=0x00000001`
    - motor1 `warmup=0x000001f4 read_speed=0x000001f4 read_enable=0x00000001`
- This is a strong confirmation that the merged all-4 readback failure still depends on Pico1's MISO electrical participation. With Pico1 removed from MISO, the secondary side becomes clean again even in the combined test.
- The symmetric isolation rerun was then performed with only Pico1 MISO connected.
- Result in that state:
  - primary readback completed cleanly in the same merged all-4 mode:
    - motor0 `expected_speed=0x00000123 read_speed=0x00000123 read_enable=0x00000001`
    - motor2 `expected_speed=0x00000789 read_speed=0x00000789 read_enable=0x00000001`
  - secondary readback collapsed to zeros:
    - motor3 `warmup=0x00000000 read_speed=0x00000000 read_enable=0x00000000`
    - motor1 `warmup=0x00000000 read_speed=0x00000000 read_enable=0x00000000`
- Together with the previous Pico1-MISO-disconnected test, this is now the clearest possible symmetry check: each side reads back correctly when it is the only active MISO participant, and the combined failure only appears when both slaves share the MISO node.

## Next Resume Step
When work resumes:
1. Treat Pico2 transport as re-proven when Pico1 MISO is absent from the shared bus.
2. Treat Pico1 GP19/MISO as the confirmed interference source in the combined setup.
3. Treat the series-resistor attempt as insufficient.
4. The next engineering choices are now:
  - inspect Pico1 GP19 hardware path for pull/load/board-level coupling
  - install the ordered tri-state buffer hardware before spending more effort on merged shared-MISO readback software sequencing

## Final All-4 Plus Switch Test Result
- A dedicated final RW612 image was built and run with marker `AUTO_TEST_ALL4_SWITCH_V1_2026_05_26`.
- Phase 1/2 reran the merged all-4 live readback immediately before the switch checks.
- Important bench condition for that run: Pico2 MISO was disconnected.
- That phase reconfirmed the shared-MISO blocker is still present in the combined configuration:
  - primary motor2 still read back correctly with `expected_speed=0x00000789 read_speed=0x00000789 read_enable=0x00000001`
  - primary motor0 speed was corrupted in this run, reading `0x00000423` instead of `0x00000123`
  - both secondary probes collapsed to zeros again:
    - motor3 `warmup=0x00000000 read_speed=0x00000000 read_enable=0x00000000`
    - motor1 `warmup=0x00000000 read_speed=0x00000000 read_enable=0x00000000`
- Because Pico2 MISO was disconnected, the secondary all-zero readback in this specific run is expected and should not be treated as a fresh discriminator for shared-bus behavior.
- Phase 2/2 then ran the guided end-switch sequence.
- Observed switch outcomes:
  - motor0 MIN test on GPIO1 triggered GPIO2 instead
  - motor0 MAX test on GPIO2 triggered GPIO1 instead
  - motor1 MIN test on GPIO15 triggered GPIO4 instead
  - motor1 MAX test on GPIO4 triggered GPIO15 instead
  - motor2 MIN on GPIO5 passed
  - motor2 MAX on GPIO11 passed
- Summary from the bench log: `pass=2 fail=4`.
- Interpretation:
  - the switch sequence still served its intended purpose as a quick "does every switch fire" check
  - switch label/order mismatches from this run should not be overinterpreted yet, because the current goal was presence detection rather than final mechanical mapping
  - move to an active isolation fix on MISO, such as a tri-state buffer or analog switch gated by CS
  - only keep probing firmware if you specifically want root-cause understanding of the RP2040 pad behavior

## Latest Combined Readback Confirmation
- A later rerun of the combined all-4 readback again showed the same split result:
  - primary readback passed cleanly in the merged run:
    - motor0 `expected_speed=0x00000123 read_speed=0x00000123 read_enable=0x00000001`
    - motor2 `expected_speed=0x00000789 read_speed=0x00000789 read_enable=0x00000001`
  - secondary readback again collapsed to zeros in that same run:
    - motor3 `warmup=0x00000000 read_speed=0x00000000 read_enable=0x00000000`
    - motor1 `warmup=0x00000000 read_speed=0x00000000 read_enable=0x00000000`
- This is another straight confirmation of the existing diagnosis rather than a new behavior change: in the combined shared-MISO setup, primary can still look clean while the secondary path collapses.

## Short Confirmation Mode
- After saving that rerun, the RW612 test image was simplified for faster bench iteration.
- New active image marker:
  - `AUTO_TEST_SHORT_CONFIRM_V1_2026_05_26`
- New active mode intent:
  - start all 4 motors
  - perform one compact readback pass for motors 0, 2, 3, and 1
  - keep the motors running for at most 3 seconds
  - stop a motor immediately if one of its configured end switches fires during that window
  - stop all remaining motors at the 3-second limit
- Log-shortening change:
  - provisional interleaved per-attempt readback logs were gated off in `dbus_driver.c`
  - final `DBCDRV_readReg32 summary` lines are still kept so each read result remains visible without the long retry spam
- The new short confirmation image rebuilt successfully and is ready for the next flash/test cycle.

## Short Confirmation Isolation Results
- The short all-4 mode was then run with only Pico1 MISO connected.
- In that configuration:
  - the switch phase still worked and motor2 stopped on GPIO11 during the 3-second window
  - the later stopped readback phase did not give a clean primary confirmation:
    - motor0 enable read errored
    - motor2 speed read back as `0x00001c89` instead of `0x00000789`
  - secondary motors 1 and 3 read back zeros, which is expected with Pico2 MISO disconnected
- The same short all-4 mode was then run with only Pico2 MISO connected.
- In that configuration:
  - primary motors 0 and 2 did not read back cleanly, which is expected with Pico1 MISO disconnected
  - secondary stopped readback did confirm the written speeds:
    - motor1 `expected=0x000001f4 speed=0x000001f4`
    - motor3 `expected=0x0000044c speed=0x0000044c`
- Interpretation:
  - the short all-4 mode remains useful for quick switch checks and for confirming the Pico2 side in isolation
  - it is not a clean apples-to-apples replacement for the earlier primary isolation readback proof, because the new sequence did not reproduce a clean primary-only stopped readback result

## New Active Primary Isolation Mode
- To restore a symmetric, directly comparable primary isolation check, a separate ultra-short primary-only stopped-readback image was added and made active.
- New active image marker:
  - `AUTO_TEST_PRIMARY_STOPPED_V1_2026_05_26`
- This active mode reuses the existing primary register log probe for motors 0 and 2 only, with no switch window and no all-4 run phase.
- The image rebuilt successfully and is now the correct next flash target for a clean Pico1-only stopped readback confirmation.

## Primary-Only Stopped Readback Confirmation
- The dedicated primary-only stopped-readback image was then run and produced a clean confirmation for Pico1-only isolated readback.
- Result:
  - motor0 `wrote_speed=0x00000123 read_speed=0x00000123 read_enable=0x00000000`
  - motor2 `wrote_speed=0x00000789 read_speed=0x00000789 read_enable=0x00000000`
- In this mode, `read_enable=0x00000000` is expected because the probe writes speed while leaving enable cleared.
- This restores the symmetric isolation result set:
  - Pico1-only stopped readback can be clean
  - Pico2-only stopped readback can be clean
  - the combined shared-MISO configuration is still the case that fails

## Latest Both-MISO Short Confirmation Run
- The short all-4 confirmation image was then rerun with both Pico MISO lines connected.
- During the 3-second motion phase, switch handling worked as intended:
  - motor1 stopped on GPIO4
  - motor0 stopped on GPIO2
  - switch summary was `motor0=1 motor1=1 motor2=0`
- The later stopped write/readback phase was mixed again rather than clean:
  - motor0 enable read failed, so there was no clean confirmation for motor0
  - motor2 speed read came back corrupted as `0x00000489` instead of `0x00000789`
  - motor1 speed readback was clean with `0x000001f4`
  - motor3 speed read failed
- This is still consistent with the established shared-MISO diagnosis:
  - combined readback is unstable and can fail in different slots on different runs
  - the isolated single-MISO cases can be clean, but the both-connected case is not reliable enough to treat as working

## Latest Isolated Short-Mode Reruns
- The short all-4 confirmation image was then rerun again in the single-MISO isolation setups.
- With only Pico1 MISO connected:
  - the 3-second switch phase completed with no switch hits in the logged rerun
  - primary stopped readback was mixed:
    - motor0 speed read back cleanly as `0x00000123`
    - motor2 enable read failed, so there was no clean confirmation for motor2
  - secondary remained unusable in that same run:
    - motor1 enable read failed
    - motor3 speed read back as `0x00000000`
- With only Pico2 MISO connected:
  - the 3-second switch phase stopped motor0 on GPIO2
  - primary stopped readback failed, which is expected with Pico1 MISO absent:
    - motor0 enable read failed
    - motor2 enable read failed
  - secondary stopped readback was clean again:
    - motor1 `expected=0x000001f4 speed=0x000001f4`
    - motor3 `expected=0x0000044c speed=0x0000044c`
- Interpretation:
  - the short all-4 mode remains a good practical switch-plus-secondary check
  - the dedicated primary-only stopped-readback image is still the authoritative Pico1 isolation proof
  - the short all-4 mode is not stable enough to replace the dedicated primary-only isolation check

## Hardware Purchase Recommendation
- Passive fixes are now exhausted enough that the next justified step is active MISO isolation.
- Recommended parts to buy:
  - `2x SN74LVC1G125` or, more conveniently, `1x SN74LVC2G125`
  - `SOT-23-5` or `VSSOP-8` breakout / adapter boards if the package is too small for direct wiring
  - `100 nF` ceramic decoupling capacitor(s)
  - small perfboard or adapter PCB for hand wiring
- Why breakout/adapter boards are needed:
  - the logic chips are tiny SMD parts
  - they are much easier to wire if first soldered onto a small adapter with larger pads or 2.54 mm header spacing

## Planned Active-Isolation Wiring
- One tri-state buffer per Pico MISO branch.
- For each branch:
  - Pico GP19 -> buffer input `A`
  - buffer output `Y` -> shared MISO node
  - that Pico's CS -> buffer `/OE`
  - `VCC` -> 3.3V
  - `GND` -> common ground
- Shared MISO node then goes to RW612 GPIO8.
- This uses hardware to physically disconnect the unselected Pico from the MISO line instead of relying on RP2040 pad behavior.

## Additional End-Switch Plan
- User plans to add four more RW612-side end switches: two for motor1 and two for motor2.
- Recommended next RW612 GPIO assignment:
  - motor1 min/home -> GPIO3
  - motor1 max/end -> GPIO4
  - motor2 min/home -> GPIO5
  - motor2 max/end -> GPIO11
- Reason for this recommendation:
  - avoids the current SPI pins GPIO6-10
  - avoids already-used motor0 end-switch pins GPIO1-2
  - avoids GPIO18, which is already reserved in the project
  - uses GPIO11 because it is available and practical in the current physical wiring plan
- Electrical recommendation stays the same as motor0:
  - input with pull-up
  - switch wired between GPIO and GND
  - active-low logic

### Current Firmware Test Flow
- RW612 runtime guard logic now covers motor0, motor1, and motor2 using the six configured GPIOs above.
- A dedicated short guided test mode now exists behind `DBUS_ENABLE_ENDSWITCH_TEST` in `zephyr_dbus_driver/src/main.c`.
- That guided mode is intentionally finite and log-light:
  - short all-4-motor preview run first
  - then 6 separate single-switch runs:
    - motor0 on GPIO1
    - motor0 on GPIO2
    - motor1 on GPIO3
    - motor1 on GPIO4
    - motor2 on GPIO5
    - motor2 on GPIO11
- Each single-switch step gives a short arm window, then runs one motor briefly until the named switch stops it or a timeout is reported.

---

## Expected Validation Outcome
Secondary-only readback has now passed with only Pico2 powered, so the remaining issue is confirmed to be a secondary-selected integrated interaction caused by Pico1 being present and powered.
That next isolation test has now completed and confirmed Pico1's physical MISO connection is the specific trigger.

---

## If It Still Fails
The next debugging branch should stay narrow:
1. verify the new UF2s were actually flashed onto both Picos
2. compare integrated readback result with only one Pico powered versus both powered
3. inspect whether inactive CS handling and MISO release occur exactly as expected on each Pico
4. only then revisit RW612-side timing or transfer sequencing

---

## Resume Summary
The session state is now stronger and narrower:
- the original integrated image has been revalidated
- quad motor control passes
- integrated primary readback passes with both Picos powered
- integrated secondary readback fails when Pico1 is also powered
- secondary readback passes when Pico1 power is removed and only Pico2 is powered
- reflashing both Picos with the stronger inactive-MISO firmware did not change that remaining failure
- keeping Pico1 powered but disconnecting only Pico1 GP19/MISO makes secondary readback pass again
- adding passive series resistors on the MISO branches did not resolve that remaining failure
- the remaining issue is pinned specifically to Pico1's MISO electrical path, and the next fix likely needs active isolation rather than a passive resistor-only network
- future reflashing convenience is now improved by the Pico USB `BOOTSEL` command and the Windows helper scripts for Pico1/Pico2

## LCD + Sonic Validation on Pico1
- The RW612 `LCD_SONIC_TEST` path was rerun after updating Pico1 to the new sniffer build `dbal_motor_v1_onehot_v3_isoD_v23_2026-05-28`.
- Pico1 logs then confirmed end-to-end LCD DBAL handling on real hardware:
  - `svc=0x7102 cmd=0x0002` with `lcd print row=1 col=0 text='starting...'`
  - `svc=0x7102 cmd=0x0002` with `lcd print row=0 col=0 text='LCD UPDATE'`
  - `svc=0x7102 cmd=0x0001` with `lcd clear`
  - `svc=0x7102 cmd=0x0002` with `lcd print row=1 col=0 text='   0 mm'`
- The user visually confirmed that the LCD now shows `LCD UPDATE ...`.
- Interpretation:
  - the earlier stuck-display problem was not in RW612 service generation
  - the root cause was Pico1 dropping DBAL frames longer than 8 bytes in the GPIO sniff path
  - after allowing the sniffer to pass DBAL-length frames into `process_dbal_frame()`, the LCD path worked on the physical module
- Remaining nuance:
  - the legacy `fw=MAIN_HWSSEL_V22_2026-05-22` banner still appears in the heartbeat log, but the active Pico firmware version string is `dbal_motor_v1_onehot_v3_isoD_v23_2026-05-28`, which confirms the new image is running

## 2026-05-28 LCD + Sonic Follow-Up Fixes
- Pico ultrasonic timing was hardened after mixed `0 mm` and impossible large values appeared during the LCD+sonic mode.
- Pico-side changes:
  - moved ultrasonic echo timestamping from the busy main loop into GPIO edge IRQ handling
  - added plausibility filtering for distance readings
  - reduced the trigger period from `100 ms` to `60 ms`
  - shortened stale-value hold from `3` missed cycles to `2`
- Effect of the Pico changes:
  - bogus spikes such as `41000 mm` stopped
  - the sensor value became correct again and started updating more responsively
- RW612 LCD mode was then simplified and hardened:
  - build marker advanced to `LCD_SONIC_TEST_V2_2026_05_28`
  - alternating `SONIC TEST` / `LCD UPDATE` header text was removed
  - LCD clear-on-every-cycle behavior was removed
  - the display now continuously re-sends a fixed `DISTANCE` header and a live value line
- RW612 hot-path logging was reduced for the LCD/sonic loop:
  - routine DBAL event send `printk` spam was disabled
  - `spi_abstraction` debug transceive logs were dropped from the hot path
- Effect of the RW612 changes:
  - visible LCD refresh improved from very slow multi-second stalls to roughly one update every ~2 seconds in bench observation
  - RW612 UART confirmed stable live reads such as `575 mm`, `583 mm`, `579 mm`, `588 mm`
- Remaining display issue after those fixes:
  - the LCD still showed `DISTANCE       Y` because the RW612 LCD service truncates each print to `LCD_TEXT_MAX_LEN = 13`, so a single padded header write did not fully erase the old `READY` tail from the Pico boot text
- Latest local fix prepared:
  - row 0 is now written as two separate DBAL prints: `DISTANCE` at column 0 plus spaces at column 8, so all 16 columns on the top row are explicitly refreshed despite the 13-character service limit

## 2026-05-28 Stable Sonic LCD State And Integrated Test Prep
- Final LCD/sonic stabilization result on Pico1:
  - Pico1 now drives the LCD locally from the sonic measurement instead of depending on the RW612 read-plus-DBAL round trip for visible value updates
  - local LCD refresh was capped to `150 ms`, which targets a readable `5-10 Hz` update rate
  - RW612 no longer writes `err=1` to the LCD; it keeps the last good distance on transient read failures
- User confirmed the distance display now behaves correctly and requested an integrated next step.
- The next active RW612 image was therefore switched away from standalone `LCD_SONIC_TEST_V2` and into a combined bench mode:
  - build marker `ALL_TOGETHER_V1_2026_05_28`
  - mode label `ALL_TOGETHER_V1`
  - RW612 runs the existing all-4 short confirmation plus endstop-aware motor test path
  - Pico1 keeps the already-proven local sonic distance display on the LCD during that run
- Intended integrated behavior of `ALL_TOGETHER_V1`:
  - all 4 motors run in the short confirmation flow
  - configured end switches can stop the mapped motors early
  - Pico1 LCD continues showing live sonic distance concurrently
