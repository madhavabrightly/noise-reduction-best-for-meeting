# Architecture

MicShieldDSP is split into application, audio, buffer, DSP, IO, logging, metrics, config, and utility layers.

The miniaudio callback is the producer. It converts input to mono float and pushes fixed-size `AudioFrame` objects into `AudioRingBuffer`. The processing thread is the consumer. It records raw audio, runs `DspChain`, records processed audio, and optionally sends processed frames to a monitoring ring buffer.

For use with external recording apps, processed audio must be routed to a playback endpoint that feeds a virtual microphone/cable. MicShieldDSP can then set the matching virtual capture endpoint as the Windows default recording and communications device. Apps that use the default microphone receive cleaned audio automatically.

The ring buffer exists so the real-time callback never waits for disk IO, logging, DSP work, or locks. Callback failures are represented by atomic overflow counters and reported later by the processing thread.

Logging is forbidden in the callback because logger sinks can allocate, lock, flush, rotate files, or touch the console.
