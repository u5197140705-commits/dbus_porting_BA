@echo off
setlocal

set "PS_SCRIPT=%~dp0flash_pico_bootsel.ps1"
set "COMPORT=COM10"

if not "%~1"=="" set "COMPORT=%~1"

if not exist "%PS_SCRIPT%" (
    echo [ERROR] Script-Datei nicht gefunden: %PS_SCRIPT%
    exit /b 1
)

echo Flashing Pico1 via serial BOOTSEL command on %COMPORT%...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%PS_SCRIPT%" -Target pico1 -ComPort "%COMPORT%"
set "RET=%ERRORLEVEL%"

if not "%RET%"=="0" (
    echo [ERROR] Pico1 flash failed. Exit code: %RET%
    exit /b %RET%
)

echo [OK] Pico1 flash complete.
exit /b 0