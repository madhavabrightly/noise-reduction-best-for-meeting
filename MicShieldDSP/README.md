# MicShieldDSP

MicShieldDSP is a Windows-first C++20 live microphone cleanup engine for meetings. It captures a real microphone, converts it to float32 mono, runs a real-time DSP chain, records raw and processed WAV files for debugging, and can route the cleaned signal into a virtual microphone such as VB-CABLE for WhatsApp, Teams, Zoom, Discord, or browser meetings.

## What It Does

- Captures normal laptop microphone or USB headset microphone.
- Processes audio continuously while the app is running.
- Uses a real-time-safe callback that only copies samples into a ring buffer.
- Runs noise reduction, hum filtering, voice activity detection, expander, optional de-esser/compressor, and limiter outside the callback.
- Writes logs under `runtime/logs`.
- Writes raw and processed comparison recordings under `runtime/recordings`.
- Can send cleaned audio to `CABLE Input` and set `CABLE Output` as the Windows default recording device.

## Important Limit

Windows does not allow a normal user-mode app to secretly replace every app's manually selected microphone. To make meeting apps receive cleaned audio, use a virtual cable:

```text
USB/normal mic -> MicShieldDSP -> CABLE Input -> CABLE Output -> WhatsApp/meeting app
```

Apps should use either `Default microphone` after MicShieldDSP sets it, or directly select `CABLE Output (VB-Audio Virtual Cable)`.

## Requirements

- Windows 10/11
- CMake 3.21 or newer
- Visual Studio 2022/2026 C++ build tools or another C++20 compiler
- Internet access during first CMake configure, because dependencies are fetched automatically
- VB-CABLE for meeting-app routing: https://vb-audio.com/Cable/

## Build

From the `MicShieldDSP` folder:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

If you use Visual Studio 2026:

```powershell
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Release
```

Run tests:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

## Check Devices

List devices visible to MicShieldDSP:

```powershell
.\build\Release\MicShieldDSP.exe --list-devices
```

Check whether your USB headset and VB-CABLE routing are ready:

```powershell
.\scripts\check_audio_setup.ps1
```

Expected ready state:

```text
OK: USB headset microphone found
OK: Virtual playback device found: CABLE Input
OK: Virtual recording device found: CABLE Output
```

## Run For USB Headset Meetings

The included USB meeting config is tuned for:

```text
Headset Microphone (AB17X USB Audio)
```

Start background filtering:

```powershell
.\scripts\start_background.ps1
```

or explicitly:

```powershell
.\build\Release\MicShieldDSP.exe config\usb_virtual_cable_config.json --background
```

Stop it:

```powershell
.\scripts\stop_background.ps1
```

Double-click button scripts are also available:

```text
scripts\Start_MicShieldDSP.cmd
scripts\Pause_MicShieldDSP.cmd
scripts\Resume_MicShieldDSP.cmd
scripts\Stop_MicShieldDSP.cmd
```

`Start_MicShieldDSP.cmd` checks for an existing `MicShieldDSP.exe` process first, so pressing it twice will not start duplicate filters.

## Run For Normal Microphone

In `config/default_config.json`, leave:

```json
"inputDeviceName": ""
```

Then run:

```powershell
.\build\Release\MicShieldDSP.exe config\default_config.json --background
```

## WhatsApp Voice Note Test

1. Install VB-CABLE and restart Windows.
2. Run:

```powershell
.\scripts\check_audio_setup.ps1
```

3. Start MicShieldDSP:

```powershell
.\scripts\start_background.ps1
```

4. In WhatsApp, select microphone:

```text
CABLE Output (VB-Audio Virtual Cable)
```

If WhatsApp does not expose microphone settings, use Windows default microphone. MicShieldDSP will try to set `CABLE Output` as the default recording and communications device when configured.

5. Send a voice note and compare it against the raw microphone.

## Configuration

Main files:

```text
config/default_config.json
config/usb_virtual_cable_config.json
```

Useful audio settings:

```json
"inputDeviceName": "Headset Microphone (AB17X USB Audio)",
"outputDeviceName": "CABLE Input",
"enableOutputMonitoring": true
```

Useful runtime settings:

```json
"runInBackground": true,
"setAsDefaultRecordingDevice": true,
"defaultRecordingDeviceName": "CABLE Output"
```

Useful DSP settings:

```json
"highPassHz": 80.0,
"humFrequencyHz": 50.0,
"noiseReductionStrength": 0.65,
"maxNoiseReductionDb": 22.0,
"gateOpenDb": -38.0,
"gateCloseDb": -48.0
```

## Architecture

```text
Application
  -> AudioEngine
  -> MiniaudioBackend
  -> AudioRingBuffer
  -> DspChain
  -> Metrics
  -> WavWriter
  -> Logger
```

The microphone callback is intentionally tiny. It converts input to mono float and pushes frames into a preallocated SPSC ring buffer. The processing thread pops frames, writes raw audio, runs DSP, writes processed audio, and optionally routes the cleaned signal to the virtual cable.

## DSP Pipeline

```text
Mic
-> Float32 mono conversion
-> DC blocker
-> High-pass 80 Hz
-> 50 Hz notch and harmonics
-> RMS meter
-> Noise estimator
-> Voice activity detector
-> Broadband noise reducer
-> Downward expander
-> Optional de-esser
-> Optional compressor
-> Limiter
-> WAV recorder / virtual output
```

The compressor is intentionally after denoising. Putting compression before noise reduction raises room noise and makes meetings sound worse.

## Runtime Files

These are generated and ignored by Git:

```text
build/
runtime/logs/
runtime/recordings/
```

Delete `runtime/recordings/*.wav` whenever you no longer need debugging comparisons. WAV files become large quickly.

## Common Problems

### CABLE Input / CABLE Output Missing

Install the full VB-CABLE ZIP as administrator, not only the setup EXE. The extracted folder must contain the `.sys` and `.cat` driver files. Restart Windows after install.

### WhatsApp Still Sounds Noisy

WhatsApp is probably using the raw USB microphone. Change its microphone to:

```text
CABLE Output (VB-Audio Virtual Cable)
```

or use `Default microphone` after MicShieldDSP starts.

### MicShieldDSP Is Not Running

Check:

```powershell
Get-Process MicShieldDSP
```

Start:

```powershell
.\scripts\start_background.ps1
```

### Repo Became Huge

Do not commit `build/`, `runtime/`, `.wav`, `.obj`, `.lib`, or `.exe` files. They are generated outputs and are ignored by `.gitignore`.
