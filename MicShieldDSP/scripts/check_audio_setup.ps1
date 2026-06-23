$root = Split-Path -Parent $PSScriptRoot
$exe = Join-Path $root "build/Release/MicShieldDSP.exe"

if (-not (Test-Path $exe)) {
    throw "MicShieldDSP.exe was not found. Build the Release target first."
}

Write-Host "MicShieldDSP devices:"
& $exe --list-devices

$endpoints = Get-PnpDevice -Class AudioEndpoint -Status OK | Select-Object -ExpandProperty FriendlyName
$hasUsbMic = $endpoints | Where-Object { $_ -like "*Headset Microphone (AB17X USB Audio)*" }
$hasCableInput = $endpoints | Where-Object { $_ -like "*CABLE Input*" }
$hasCableOutput = $endpoints | Where-Object { $_ -like "*CABLE Output*" }

Write-Host ""
Write-Host "Setup check:"
if ($hasUsbMic) {
    Write-Host "OK: USB headset microphone found: $hasUsbMic"
} else {
    Write-Host "MISSING: USB headset microphone 'Headset Microphone (AB17X USB Audio)'"
}

if ($hasCableInput) {
    Write-Host "OK: Virtual playback device found: $hasCableInput"
} else {
    Write-Host "MISSING: Virtual playback device 'CABLE Input'"
}

if ($hasCableOutput) {
    Write-Host "OK: Virtual recording device found: $hasCableOutput"
} else {
    Write-Host "MISSING: Virtual recording device 'CABLE Output'"
}

Write-Host ""
if ($hasUsbMic -and $hasCableInput -and $hasCableOutput) {
    Write-Host "Ready: run scripts/start_background.ps1, then record in WhatsApp using Default microphone or CABLE Output."
} else {
    Write-Host "Not ready for WhatsApp filtering yet. Install/enable a virtual audio cable so Windows has CABLE Input and CABLE Output."
}
