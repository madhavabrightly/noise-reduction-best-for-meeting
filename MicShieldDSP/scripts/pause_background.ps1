$running = Get-Process MicShieldDSP -ErrorAction SilentlyContinue
if (-not $running) {
    Write-Host "MicShieldDSP is not running."
    exit 0
}

Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public static class NativeSuspendProcessControl {
    [DllImport("ntdll.dll")]
    public static extern int NtSuspendProcess(IntPtr processHandle);
}
"@ -ErrorAction SilentlyContinue

foreach ($process in $running) {
    $result = [NativeSuspendProcessControl]::NtSuspendProcess($process.Handle)
    if ($result -eq 0) {
        Write-Host "Paused MicShieldDSP process $($process.Id)."
    } else {
        Write-Host "Could not pause MicShieldDSP process $($process.Id). Native result: $result"
    }
}
