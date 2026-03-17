@echo off
setlocal

set "JLINK_EXE=C:\Program Files\SEGGER\JLink_V924a\JLink.exe"
set "SCRIPT_FILE=%~dp0flash_rw612.jlink"

if not exist "%JLINK_EXE%" (
    echo [ERROR] J-Link nicht gefunden: %JLINK_EXE%
    exit /b 1
)

if not exist "%SCRIPT_FILE%" (
    echo [ERROR] Script-Datei nicht gefunden: %SCRIPT_FILE%
    exit /b 1
)

echo Starte J-Link Flash fuer RW612...
"%JLINK_EXE%" -CommanderScript "%SCRIPT_FILE%"

set "RET=%ERRORLEVEL%"
if not "%RET%"=="0" (
    echo [ERROR] Flash fehlgeschlagen. Exit code: %RET%
    exit /b %RET%
)

echo [OK] Flash abgeschlossen.
exit /b 0
