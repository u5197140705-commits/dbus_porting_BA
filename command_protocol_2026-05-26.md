# Command Protocol — 2026-05-26

## Objective
Keep a separate, bench-focused record of reusable commands so reflashing, validation, and recovery steps are easy to find without scanning full session protocols.

---

## Saved Commands

### Primary Pico: reboot to BOOTSEL without reconnecting
Purpose:
- send the USB serial `BOOTSEL` command to Pico1 so it re-enumerates as `RPI-RP2`

Command:
```powershell
powershell.exe -NoProfile -Command '$port = [System.IO.Ports.SerialPort]::new("COM10",115200); $port.NewLine = "`r`n"; $port.DtrEnable = $true; $port.RtsEnable = $true; $port.Open(); Start-Sleep -Milliseconds 200; $port.WriteLine("BOOTSEL"); $port.BaseStream.Flush(); $port.Close(); Write-Output "BOOTSEL sent to COM10"'
```

Follow-up:
- copy `pico_spi_slave_test/build/pico_spi_slave_test.uf2` to the `RPI-RP2` drive after it appears

Notes:
- current repo mapping is Pico1 = `COM10`
- this matches the USB command handler implemented in `pico_spi_slave_test/main.c`

### Primary Pico: reboot to BOOTSEL and open the `RPI-RP2` drive
Purpose:
- send the USB serial `BOOTSEL` command to Pico1, wait for the mass-storage device, and open it directly in Windows Explorer for manual copy

Command:
```powershell
powershell.exe -NoProfile -Command '$ErrorActionPreference = "Stop"; $port = [System.IO.Ports.SerialPort]::new("COM10",115200); try { $port.NewLine = "`r`n"; $port.DtrEnable = $true; $port.RtsEnable = $true; $port.Open(); Start-Sleep -Milliseconds 500; $port.WriteLine("BOOTSEL"); $port.BaseStream.Flush(); } finally { if ($port.IsOpen) { $port.Close() } }; $deadline = (Get-Date).AddSeconds(40); $driveRoot = $null; while ((Get-Date) -lt $deadline) { $disk = Get-CimInstance Win32_LogicalDisk | Where-Object { $_.VolumeName -eq "RPI-RP2" } | Select-Object -First 1; if ($disk) { $driveRoot = $disk.DeviceID + "\\"; break }; Start-Sleep -Milliseconds 500 }; if (-not $driveRoot) { throw "Timed out waiting for RPI-RP2" }; Start-Process explorer.exe -ArgumentList "/root,$driveRoot"; Write-Output "Opened $driveRoot"'
```

Follow-up:
- drag [pico_spi_slave_test/build/pico_spi_slave_test.uf2](/home/swied/projects/dbus_porting_BA/pico_spi_slave_test/build/pico_spi_slave_test.uf2) into the opened `RPI-RP2` window

Notes:
- if `Access to the port 'COM10' is denied`, a serial monitor or other Windows process still has Pico1 open
- after freeing `COM10`, rerun this command instead of the plain BOOTSEL one
- on this machine, this variant can still open an extra Documents window, so do not use it by default

### Preferred default for Pico1 flashing
Purpose:
- enter BOOTSEL only, then let the user copy the UF2 manually without any automatic Explorer launch

Command:
```powershell
powershell.exe -NoProfile -Command '$port = [System.IO.Ports.SerialPort]::new("COM10",115200); $port.NewLine = "`r`n"; $port.DtrEnable = $true; $port.RtsEnable = $true; $port.Open(); Start-Sleep -Milliseconds 200; $port.WriteLine("BOOTSEL"); $port.BaseStream.Flush(); $port.Close(); Write-Output "BOOTSEL sent to COM10"'
```

Notes:
- this is now the preferred default because it avoids the unwanted extra Documents window
- only use the auto-open variant if explicitly requested

### Secondary Pico: reboot to BOOTSEL without reconnecting
Purpose:
- send the USB serial `BOOTSEL` command to Pico2 so it re-enumerates as `RPI-RP2`

Command:
```powershell
powershell.exe -NoProfile -Command '$port = [System.IO.Ports.SerialPort]::new("COM9",115200); $port.NewLine = "`r`n"; $port.DtrEnable = $true; $port.RtsEnable = $true; $port.Open(); Start-Sleep -Milliseconds 200; $port.WriteLine("BOOTSEL"); $port.BaseStream.Flush(); $port.Close(); Write-Output "BOOTSEL sent to COM9"'
```

Follow-up:
- copy `pico_spi_slave_test/build/pico_spi_slave_test_pico2.uf2` to the `RPI-RP2` drive after it appears

Notes:
- current repo mapping is Pico2 = `COM9`
- this uses the same USB command handler implemented in `pico_spi_slave_test/main.c`

### RW612: rebuild current integrated image
Purpose:
- rebuild the active RW612 Zephyr application in the current `build_local` directory

Command:
```bash
cmake --build zephyr_dbus_driver/build_local -j
```

Expected output artifact:
- `zephyr_dbus_driver/build_local/zephyr/zephyr.elf`

Notes:
- this is the build path used for the current integrated RW612 test image

### RW612: flash current integrated image with saved J-Link wrapper
Purpose:
- flash the current `build_local` RW612 image through the existing Windows J-Link script

Command:
```cmd
scripts\flash_rw612.cmd
```

Direct invocation from this WSL workspace:
```bash
cmd.exe /c scripts\\flash_rw612.cmd
```

Notes:
- wrapper script points at `zephyr_dbus_driver/build_local/zephyr/zephyr.elf`
- wrapper uses the saved J-Link installation path and `scripts/flash_rw612.jlink`
- for isolated Pico1/Pico2 readback conclusions, RW612 target selection alone is not enough; the other Pico's MISO wire must be physically disconnected from the shared bus

### 2026-07-03 isolated Pico2 readback conclusion
Purpose:
- record the final practical conclusion from the isolated Pico2 readback investigation before pivoting back to simultaneous testing

Summary:
- accepted working scope:
	- writes work
	- primary readback worked in the validated May 28 baseline
	- secondary enable readback worked in the validated May 28 baseline
- not accepted as robust:
	- secondary speed readback
- latest isolated Pico2 debug result (`v54 cmd1latch`) showed:
	- one lone `0x40` command byte could be latched (`c1l=1`)
	- no recovered promoted frame followed (`c1p=0`)
	- stale single-byte drops were still dominated by `0x00`
- practical conclusion:
	- Pico2 speed readback is still transport/framing-limited and not reliable enough to use as a release criterion
	- for time-constrained continuation, use secondary enable readback as the practical validated readback signal and treat secondary speed readback as a known unresolved limitation

Why primary readback works more fully than secondary readback:
- primary Pico is selected through the RW612 hardware chip-select path on GPIO6 / `SSEL0`
- secondary Pico is selected through RW612 GPIO10 as a manual GPIO chip-select path
- validated bench evidence showed the primary path can return stable contiguous readback while the secondary path often depends on fragmented provisional/interleaved payload recovery
- validated bench evidence also showed the secondary path is much more sensitive to bus topology:
	- secondary readback improved when Pico1 MISO was removed from the bus
	- secondary readback could still vary by probe order and timing even after that
- practical interpretation:
	- the unresolved difference is not a simple Pico1-vs-Pico2 firmware feature gap
	- the unresolved difference is the secondary transport path itself: manual-CS framing plus shared-bus/MISO interaction makes secondary readback less stable and less symmetric than the primary hardware-CS path

### RW612: one last isolated Pico2 stopped-readback retest
Purpose:
- run one final isolated Pico2 retest using the saved May 28 stopped-readback RW612 image before switching focus back to simultaneous testing

Command:
```bash
cp /home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/rw612_isolated_pico2_stopped_readback_2fd321d.elf /home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_local/zephyr/zephyr.elf
```

Companion Pico artifacts:
- `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/pico_spi_slave_test_pico1_2fd321d.uf2`
- `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/pico_spi_slave_test_pico2_2fd321d.uf2`

Notes:
- keep Pico1 MISO physically disconnected for this isolated Pico2 test
- this is the last recommended isolated Pico2 speed-readback retest before pivoting back to simultaneous validation
- after copying the ELF, flash it with the normal RW612 wrapper command

Observed final retest result:
- RW612 runtime marker: `ISOLATED_PICO2_STOPPED_READBACK_V1_2026_05_28`
- motor1 failed completely in this stopped isolated run:
	- warmup speed readback: `0x00000000`
	- speed readback: `0x00000000`
	- enable readback: `0x00000000`
- motor3 remained readable in the same run:
	- warmup speed readback: `0x0000044c`
	- speed readback: `0x0000044c`
	- enable readback: `0x00000000`
- practical implication:
	- even the saved May 28 isolated Pico2 stopped-readback image does not provide robust symmetric secondary readback
	- secondary speed readback can work for motor3, but motor1 and secondary enable are still not dependable enough to gate the project on isolated Pico2 readback

### RW612: restore short all-4 switch-aware image
Purpose:
- restage the known-good 3-second all-4 motor test with active end-switch handling into the normal `build_local` flash path

Command:
```bash
cp /home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/rw612_short_switch_2fd321d.elf /home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_local/zephyr/zephyr.elf
```

Expected runtime markers:
- `Main: starting short QUAD switch-aware motor test (motor0..motor3)`
- `Motor Toggle [QUAD_SHORT_SWITCH_V1]`

Notes:
- this image is for the practical 3-second all-4 movement check with active switches, not for isolated readback proof
- flash it afterward with the normal RW612 wrapper command

### RW612: flash current integrated image with Linux-side helper
Purpose:
- flash the same RW612 image from Linux when `JLinkExe` is available in the shell environment

Command:
```bash
ELF_FILE=/home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_local/zephyr/zephyr.elf ./flash_dbus_driver.sh
```

Notes:
- `flash_dbus_driver.sh` defaults to `build_swied`, so override `ELF_FILE` when using `build_local`
- this path depends on `JLinkExe` being callable from Linux

### Bench power plan: RW612, Pico, motor drivers, LCD, sonic, buffer
Purpose:
- keep the logic side on regulated USB/power-bank power while the motors use a separate AA battery pack

Recommended wiring:
- USB power bank `5V` -> RW612 USB power input
- USB/power-bank regulated `5V` -> Pico `VSYS` when Pico is not USB-powered directly
- USB/power-bank regulated `5V` -> buck converter input when external `3.3V` is needed
- buck converter `3.3V` -> `74HC125` `VCC`
- buck converter `3.3V` -> motor-driver logic `VCC`
- buck converter `3.3V` -> LCD logic `VDD`
- `6 x AA` battery pack -> motor-driver `VM`
- all grounds tied together: RW612, both Picos, buffer chip, LCD, sonic sensor, motor drivers, AA battery negative, power-bank/buck ground

Notes:
- prefer a normal USB power bank for RW612 instead of back-powering the board through an arbitrary `5V0` header pin
- do not use Pico `VBUS` as the source for external `5V` peripherals when Pico is powered from `VSYS`
- RW612 `SCK` and `MOSI` can go directly to both Picos; only shared `MISO` needs the tri-state buffer isolation
- `74HC125` must run at `3.3V`, not `5V`
- `1OE` and `2OE` connect directly to the same RW612 CS lines that already go to Pico1/Pico2 CS
- tie unused `3OE` and `4OE` to `3.3V` so those channels stay disabled

Decoupling / bulk capacitors:
- `74HC125`: `100 nF` ceramic directly between `VCC` and `GND`, as close to pins `14` and `7` as possible
- each motor-driver board: `100 nF` ceramic from `VCC` to `GND`
- each motor-driver board: `100 nF` ceramic from `VM` to `GND`
- each motor-driver board: bulk capacitor from `VM` to `GND`; `47 uF` to `220 uF` is preferred, `10 uF` is acceptable temporarily
- if the bulk capacitor is electrolytic: `+` to `VM`, `-` to `GND`

Physical placement guidance:
- place motor-driver capacitors on the motor-driver module itself or on a tiny perfboard carrying that module
- keep capacitor leads short and place them close to the module `VM/GND` and `VCC/GND` connections
- for the `74HC125`, both `1Y` and `2Y` go to the same shared MISO node that then goes to RW612 `GPIO8`

### Simultaneous buffer test: practical next baseline
Purpose:
- resume with the shared-bus hardware-isolation path instead of spending more time on isolated Pico2 readback recovery

Recommended artifact set:
- RW612: `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/rw612_auto_test_all4_2fd321d.elf`
- Pico1: `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/pico_spi_slave_test_pico1_2fd321d.uf2`
- Pico2: `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/pico_spi_slave_test_pico2_2fd321d.uf2`

Preparation:
- copy the RW612 ELF into the normal flash path:
```bash
cp /home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/rw612_auto_test_all4_2fd321d.elf /home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_local/zephyr/zephyr.elf
```
- flash both saved Pico UF2 images
- install the tri-state buffer on shared `MISO` as documented above
- keep `SCK` and `MOSI` shared directly
- route each Pico `GP19` through its own buffer channel into the common RW612 `GPIO8` MISO node
- gate each buffer enable with that Pico's own CS line so only the selected Pico can drive MISO

Minimum success criteria:
- all four motors start and run in the known-good all-4 baseline
- primary readback still remains correct
- secondary readback no longer collapses to all zeros when Pico1 is also present on the bus

Interpretation:
- if primary stays correct and secondary becomes readable with the buffer in place, the remaining blocker was shared-MISO electrical interaction rather than a missing protocol-side software fix
- if secondary still collapses with active MISO isolation in place, then the next blocker is more likely on the secondary CS / transport path itself

### Simultaneous buffer test: next concrete run order
Purpose:
- turn the buffer validation into one safe immediate run plus one gated follow-up readback run that respects the new motor-speed floor

Run now:
- keep using the saved May 28 simultaneous motion baseline:
	- RW612: `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/rw612_auto_test_all4_2fd321d.elf`
	- Pico1: `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/pico_spi_slave_test_pico1_2fd321d.uf2`
	- Pico2: `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/pico_spi_slave_test_pico2_2fd321d.uf2`
- this is the correct immediate buffer sanity check because it already passed on the bench and does not depend on the unstable isolated Pico2 readback path

Do not use unchanged for the next readback proof:
- `rw612_short_switch_readback_2fd321d.elf`
- `rw612_short_switch_readback_strict_2fd321d.elf`
- reason: the historical combined readback notes for that May 20 / May 28 family include sub-400 commanded values such as `0x00000123` and `0x000001F4`, which are now below the accepted bench floor for reliable motor turning

Next readback run requirement:
- rebuild or patch the readback-capable RW612 harness so every commanded motor speed is `>= 400`
- recommended minimum replacement set for the first buffer-assisted combined readback rerun:
	- motor0: `400`
	- motor1: `500`
	- motor2: `800`
	- motor3: `1100`
- keep the practical acceptance rule unchanged:
	- primary readback should stay correct
	- secondary enable readback should become non-zero / readable with both Picos connected through the buffer
	- secondary speed readback is still useful evidence when it works, but it is not the only pass criterion

Practical sequencing:
1. Reconfirm the saved all-4 motion baseline with the buffer in place.
2. Do not spend another run on the old low-speed readback ELF variants.
3. When readback is the goal, first produce a new RW612 readback image with no commanded speed below `400`.

Observed buffer sanity rerun on 2026-07-03:
- the saved May 28 movement baseline still passes with both Picos present and the buffer installed:
	- runtime marker: `AUTO_TEST_ALL4_V1_2026_05_20`
	- summary: `PASS: 1`, `FAIL: 0`, `Overall: PASS`
- keep interpreting that run only as motion/write-path confirmation
- do not reuse it as the next readback proof because its built-in rounds still include sub-400 values (`200`, `300`, `250`)

### RW612: floor-400 all-4 live readback image
Purpose:
- provide the next combined readback RW612 image with the existing all-4 live readback sequence preserved but with no commanded speed below `400`

Built artifact:
- `/home/swied/projects/dbus_porting_BA/helper/all4_readback_floor400_v1_2026-07-03.elf`

Expected runtime markers:
- `Hello from Zephyr DBus Driver project! [ALL4_READBACK_FLOOR400_V1]`
- `RW612 build marker: ALL4_READBACK_FLOOR400_V1_2026_07_03`
- `Main: mode=ALL4_READBACK_FLOOR400_V1`
- `Main: ALL-4 live quad readback mode active`

Current speed set in this readback image:
- motor0: `400`
- motor1: `500`
- motor2: `800`
- motor3: `1100`

Stage command:
```bash
cp /home/swied/projects/dbus_porting_BA/helper/all4_readback_floor400_v1_2026-07-03.elf /home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_local/zephyr/zephyr.elf
```

Notes:
- this image keeps the current combined readback probe order and warm-up read behavior
- use the normal RW612 flash wrapper after staging it

Observed V1 result on 2026-07-03:
- runtime marker: `ALL4_READBACK_FLOOR400_V1_2026_07_03`
- outcome: no motor movement and all readback paths, including primary, returned only `ff` / header-scan miss results
- practical conclusion: the V1 all-4 live readback harness did not preserve the known-good motion-start sequence; the failure was not caused by low-speed values anymore

### RW612: floor-400 all-4 live readback image V2
Purpose:
- preserve the `>= 400` readback target set while reusing the proven `start_motor_now()` motion-start helper before any read probes

Built artifact:
- `/home/swied/projects/dbus_porting_BA/helper/all4_readback_floor400_v2_2026-07-03.elf`

Expected runtime markers:
- `Hello from Zephyr DBus Driver project! [ALL4_READBACK_FLOOR400_V2]`
- `RW612 build marker: ALL4_READBACK_FLOOR400_V2_2026_07_03`
- `Main: mode=ALL4_READBACK_FLOOR400_V2`
- `Main: ALL-4 live quad readback mode active`

Current speed set in this readback image:
- motor0: `400`
- motor1: `500`
- motor2: `800`
- motor3: `1100`

Stage command:
```bash
cp /home/swied/projects/dbus_porting_BA/helper/all4_readback_floor400_v2_2026-07-03.elf /home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_local/zephyr/zephyr.elf
```

Notes:
- V2 differs from V1 only where it matters locally: motor start now goes through the same `start_motor_now()` helper used by the proven motion path
- keep the same readback probe order for comparability with the failed V1 run

Observed V2 result on 2026-07-03:
- user observation: no motor was moving
- practical conclusion: even after switching to `start_motor_now()`, the dedicated all-4 readback mode still diverged from the proven startup path because it returned before the shared initialization sequence ran

### RW612: floor-400 all-4 live readback image V3
Purpose:
- keep the V2 `>= 400` motor set and `start_motor_now()` fix, but move the all-4 live readback mode behind the shared startup path used by the known-good baseline

Built artifact:
- `/home/swied/projects/dbus_porting_BA/helper/all4_readback_floor400_v3_2026-07-03.elf`

Expected runtime markers:
- `Hello from Zephyr DBus Driver project! [ALL4_READBACK_FLOOR400_V3]`
- `RW612 build marker: ALL4_READBACK_FLOOR400_V3_2026_07_03`
- `Main: mode=ALL4_READBACK_FLOOR400_V3`
- `Main: DBAL bootstrap disabled for SPI isolation`
- `Main: boot-time motor pre-disable done`
- `Main: ALL-4 live quad readback mode active`

Stage command:
```bash
cp /home/swied/projects/dbus_porting_BA/helper/all4_readback_floor400_v3_2026-07-03.elf /home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_local/zephyr/zephyr.elf
```

Notes:
- V3 changes only the mode entry path: the readback test now runs after the same shared startup delay, target selection, and boot-time pre-disable sequence as the proven motion baseline

Observed V3 result on 2026-07-03:
- user observation: still no motors moving
- practical conclusion: the dedicated floor-400 all-4 readback branch is suspended for now because it has not yet preserved the proven all-4 motion behavior well enough to serve as a trustworthy next readback discriminator

### RW612: restore known-good all-4 motion baseline
Purpose:
- return immediately to the last proven four-motor moving image after the failed floor-400 readback branch

Active staged artifact:
- `/home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/rw612_auto_test_all4_2fd321d.elf`

Stage command:
```bash
cp /home/swied/projects/dbus_porting_BA/helper/known_good_2026-05-28/rw612_auto_test_all4_2fd321d.elf /home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_local/zephyr/zephyr.elf
```

Expected runtime markers after flash:
- `Hello from Zephyr DBus Driver project! [AUTO_TEST_ALL4_V1]`
- `RW612 build marker: AUTO_TEST_ALL4_V1_2026_05_20`
- `Main: mode=AUTO_TEST_ALL4_V1`

Notes:
- use this as the active safe fallback whenever the immediate goal is simply to restore all four moving motors
- do not treat it as a readback proof; it remains motion/write-path evidence only

### Exact 2fd321d baseline with appended stopped readback
Purpose:
- test readback by modifying the true May 20 moving source baseline instead of the later reconstructed RW612 tree

Built artifact:
- `/home/swied/projects/dbus_porting_BA/helper/exact2fd321d_all4_stopped_readback_v1_2026-07-03.elf`

Expected runtime markers:
- `Hello from Zephyr DBus Driver project! [EXACT2FD321D_ALL4_STOPPED_READBACK_V1]`
- `RW612 build marker: EXACT2FD321D_ALL4_STOPPED_READBACK_V1_2026_07_03`
- `Main: mode=EXACT2FD321D_ALL4_STOPPED_READBACK_V1`

Intentional behavior of this test image:
- only one motion round is expected
- the speed set is fixed at the new bench-safe floor / reference set:
	- motor0: `400`
	- motor1: `500`
	- motor2: `800`
	- motor3: `1100`
- there is no multi-round speed change in this image by design

Observed result on 2026-07-03:
- the motors did move in this exact-base variant, which is the first successful proof that the motion path is preserved while stopped readback is appended afterward
- primary stopped readback passed cleanly:
	- motor0 enable read back `0x00000000`
	- motor0 speed read back `0x00000190`
	- motor2 enable read back `0x00000000`
	- motor2 speed read back `0x00000320`
- secondary stopped enable readback also became readable and returned the expected stopped value:
	- motor1 enable read back `0x00000000`
	- motor3 enable read back `0x00000000`
- secondary stopped speed readback still failed for both motors:
	- motor1 speed read failed at `0x5014`
	- motor3 speed read failed at `0x5034`

Practical conclusion:
- yes, the lack of speed change in this exact-base test was intentional
- this run is an important positive result because it preserves motion while showing a more precise split in readback behavior:
	- primary stopped readback works
	- secondary stopped enable readback works
	- secondary stopped speed readback remains the unresolved part

---

## Conventions For Future Entries
- add one command per subsection
- include purpose, exact command, and required follow-up steps
- record port or board mapping assumptions next to the command