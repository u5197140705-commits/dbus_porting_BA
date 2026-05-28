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

---

## Conventions For Future Entries
- add one command per subsection
- include purpose, exact command, and required follow-up steps
- record port or board mapping assumptions next to the command