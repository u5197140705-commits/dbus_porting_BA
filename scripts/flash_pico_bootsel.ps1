param(
    [ValidateSet("pico1", "pico2")]
    [string]$Target = "pico1",

    [string]$ComPort = "COM10",

    [int]$BaudRate = 115200,

    [int]$DriveTimeoutSec = 20
)

$ErrorActionPreference = "Stop"

$uf2Map = @{
    pico1 = "\\wsl.localhost\Ubuntu\home\swied\projects\dbus_porting_BA\pico_spi_slave_test\build\pico_spi_slave_test.uf2"
    pico2 = "\\wsl.localhost\Ubuntu\home\swied\projects\dbus_porting_BA\pico_spi_slave_test\build\pico_spi_slave_test_pico2.uf2"
}

$uf2Path = $uf2Map[$Target]
if (-not (Test-Path $uf2Path)) {
    throw "UF2 not found: $uf2Path"
}

Write-Host "[INFO] Target: $Target"
Write-Host "[INFO] COM port: $ComPort"
Write-Host "[INFO] UF2: $uf2Path"

$port = $null
try {
    $port = [System.IO.Ports.SerialPort]::new($ComPort, $BaudRate)
    $port.NewLine = "`r`n"
    $port.ReadTimeout = 500
    $port.WriteTimeout = 1000
    $port.DtrEnable = $true
    $port.RtsEnable = $true
    $port.Open()

    Start-Sleep -Milliseconds 200
    $port.WriteLine("BOOTSEL")
    $port.BaseStream.Flush()
    Write-Host "[INFO] BOOTSEL command sent. Waiting for RPI-RP2..."
}
finally {
    if ($null -ne $port -and $port.IsOpen) {
        $port.Close()
    }
}

$deadline = (Get-Date).AddSeconds($DriveTimeoutSec)
$driveRoot = $null

while ((Get-Date) -lt $deadline) {
    $disk = Get-CimInstance Win32_LogicalDisk | Where-Object { $_.VolumeName -eq "RPI-RP2" } | Select-Object -First 1
    if ($null -ne $disk) {
        $driveRoot = $disk.DeviceID + "\\"
        break
    }
    Start-Sleep -Milliseconds 250
}

if ($null -eq $driveRoot) {
    throw "Timed out waiting for RPI-RP2 after sending BOOTSEL on $ComPort"
}

Write-Host "[INFO] RPI-RP2 detected at $driveRoot"
Copy-Item -Path $uf2Path -Destination $driveRoot -Force
Write-Host "[OK] Copied $(Split-Path $uf2Path -Leaf) to $driveRoot"
