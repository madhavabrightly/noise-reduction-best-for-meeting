param(
    [string]$Config = "config/usb_virtual_cable_config.json"
)

$root = Split-Path -Parent $PSScriptRoot
$running = Get-Process MicShieldDSP -ErrorAction SilentlyContinue
if ($running) {
    Write-Host "MicShieldDSP is already running. No second copy was started."
    $running | Select-Object Id, ProcessName, Path, StartTime | Format-Table -AutoSize
    exit 0
}

$exe = Join-Path $root "bin/MicShieldDSP.exe"
if (-not (Test-Path $exe)) {
    $exe = Join-Path $root "build/Release/MicShieldDSP.exe"
}

if (-not (Test-Path $exe)) {
    throw "MicShieldDSP.exe was not found. Build the Release target first, or place it at MicShieldDSP/bin/MicShieldDSP.exe."
}

Start-Process -FilePath $exe -ArgumentList "`"$Config`" --background" -WorkingDirectory $root -WindowStyle Hidden
Write-Host "MicShieldDSP started in background mode."
