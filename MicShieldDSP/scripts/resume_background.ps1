$running = Get-Process MicShieldDSP -ErrorAction SilentlyContinue
if (-not $running) {
    Write-Host "MicShieldDSP is not running."
    exit 0
}

Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public static class NativeResumeProcessControl {
    [DllImport("ntdll.dll")]
    public static extern int NtResumeProcess(IntPtr processHandle);
}
"@ -ErrorAction SilentlyContinue

foreach ($process in $running) {
    $result = [NativeResumeProcessControl]::NtResumeProcess($process.Handle)
    if ($result -eq 0) {
        Write-Host "Resumed MicShieldDSP process $($process.Id)."
    } else {
        Write-Host "Could not resume MicShieldDSP process $($process.Id). Native result: $result"
    }
}
