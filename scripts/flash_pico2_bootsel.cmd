@echo off
setlocal

set "PS_SCRIPT=%~dp0flash_pico_bootsel.ps1"
set "COMPORT=COM9"

if not "%~1"=="" set "COMPORT=%~1"

if not exist "%PS_SCRIPT%" (
    echo [ERROR] Script-Datei nicht gefunden: %PS_SCRIPT%
    exit /b 1
)

echo Flashing Pico2 via serial BOOTSEL command on %COMPORT%...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%PS_SCRIPT%" -Target pico2 -ComPort "%COMPORT%"
set "RET=%ERRORLEVEL%"

if not "%RET%"=="0" (
    echo [ERROR] Pico2 flash failed. Exit code: %RET%
    exit /b %RET%
)

echo [OK] Pico2 flash complete.
exit /b 0