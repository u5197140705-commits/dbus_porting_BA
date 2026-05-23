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

## Next Resume Step
When work resumes:
1. Treat Pico2 transport as re-proven when Pico1 MISO is absent from the shared bus.
2. Treat Pico1 GP19/MISO as the confirmed interference source in the combined setup.
3. Treat the series-resistor attempt as insufficient.
4. The next engineering choices are now:
  - inspect Pico1 GP19 hardware path for pull/load/board-level coupling
  - move to an active isolation fix on MISO, such as a tri-state buffer or analog switch gated by CS
  - only keep probing firmware if you specifically want root-cause understanding of the RP2040 pad behavior

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
  - motor2 max/end -> GPIO13
- Reason for this recommendation:
  - avoids the current SPI pins GPIO6-10
  - avoids already-used motor0 end-switch pins GPIO1-2
  - avoids GPIO18, which is already reserved in the project
  - avoids GPIO11 and GPIO12, which were already noted as impractical choices in this board setup
- Electrical recommendation stays the same as motor0:
  - input with pull-up
  - switch wired between GPIO and GND
  - active-low logic

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
