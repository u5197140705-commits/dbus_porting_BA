@echo off
setlocal EnableDelayedExpansion

set "JLINK_EXE=C:\Program Files\SEGGER\JLink_V924a\JLink.exe"
set "SCRIPT_FILE=%~dp0flash_rw612_secondary_readback.jlink"
set "ELF_UNC=\\wsl.localhost\Ubuntu\home\swied\projects\dbus_porting_BA\zephyr_dbus_driver\build_secondary_readback\zephyr\zephyr.elf"
set "ELF_WSL=/home/swied/projects/dbus_porting_BA/zephyr_dbus_driver/build_secondary_readback/zephyr/zephyr.elf"

if not exist "%JLINK_EXE%" (
    echo [ERROR] J-Link nicht gefunden: %JLINK_EXE%
    exit /b 1
)

if not exist "%SCRIPT_FILE%" (
    echo [ERROR] Script-Datei nicht gefunden: %SCRIPT_FILE%
    exit /b 1
)

if not exist "%ELF_UNC%" (
    echo [ERROR] ELF nicht gefunden: %ELF_UNC%
    exit /b 1
)

echo ELF preflight: %ELF_UNC%
where wsl >nul 2>nul
if "%ERRORLEVEL%"=="0" (
    for /f "delims=" %%L in ('wsl bash -lc "strings \"%ELF_WSL%\" | grep -m1 \"auto motor test disabled\" || true"') do (
        set "ELF_MARKER=%%L"
    )
    if defined ELF_MARKER (
        echo ELF marker: !ELF_MARKER!
    ) else (
        echo [WARN] Kein erwarteter Marker im ELF gefunden.
    )
) else (
    echo [WARN] wsl.exe nicht gefunden, Marker-Preflight uebersprungen.
)

echo Starte J-Link Flash fuer RW612 (secondary-only readback image)...
"%JLINK_EXE%" -CommanderScript "%SCRIPT_FILE%"

set "RET=%ERRORLEVEL%"
if not "%RET%"=="0" (
    echo [ERROR] Flash fehlgeschlagen. Exit code: %RET%
    exit /b %RET%
)

echo [OK] Flash abgeschlossen.
exit /b 0
