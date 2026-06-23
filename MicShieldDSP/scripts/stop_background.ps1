Get-Process MicShieldDSP -ErrorAction SilentlyContinue | Stop-Process
Write-Host "MicShieldDSP stopped if it was running."
