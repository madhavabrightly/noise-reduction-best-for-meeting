# Logging Policy

Logs are written under `runtime/logs` using rotating spdlog sinks. Categories are app, audio, device, dsp, performance, and error.

Startup logs include configuration, selected device, sample rate, frame size, and DSP settings. Runtime performance logs include RMS, noise floor, VAD state, gain reduction, processing time, and ring buffer overflow counts.

The audio callback does not log. It only copies samples into a preallocated buffer. Any callback-side failure is represented by counters and flushed later from the processing thread.
