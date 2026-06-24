$running = Get-Process MicShieldDSP -ErrorAction SilentlyContinue
if (-not $running) {
    Write-Host "MicShieldDSP is not running."
    exit 0
}

$running | Stop-Process -Force
Write-Host "MicShieldDSP stopped."
