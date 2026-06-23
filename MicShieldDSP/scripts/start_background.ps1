param(
    [string]$Config = "config/usb_virtual_cable_config.json"
)

$root = Split-Path -Parent $PSScriptRoot
$exe = Join-Path $root "build/Release/MicShieldDSP.exe"

if (-not (Test-Path $exe)) {
    throw "MicShieldDSP.exe was not found. Build the Release target first."
}

Start-Process -FilePath $exe -ArgumentList "`"$Config`" --background" -WorkingDirectory $root -WindowStyle Hidden
Write-Host "MicShieldDSP started in background mode."
