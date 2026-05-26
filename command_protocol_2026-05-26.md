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