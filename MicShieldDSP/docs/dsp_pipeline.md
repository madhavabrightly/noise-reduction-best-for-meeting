# DSP Pipeline

1. `DCBlocker` removes microphone DC offset.
2. `HighPassFilter` removes low rumble with an 80 Hz default voice-safe cutoff.
3. `NotchFilter` removes 50 Hz hum for India plus optional harmonics at 100 Hz and 150 Hz.
4. `RmsMeter` measures pre-processing level.
5. `NoiseEstimator` tracks room noise more quickly during non-speech.
6. `VoiceActivityDetector` combines RMS, noise floor, hysteresis, and hangover.
7. `NoiseReducer` applies smooth broadband gain reduction when speech is absent or close to the noise floor.
8. `Expander` lowers residual background noise more naturally than a hard gate.
9. `DeEsser` optionally attenuates harsh 5-8 kHz energy.
10. `Compressor` optionally levels voice.
11. `Limiter` prevents clipping at -1 dBFS by default.
12. `RmsMeter` measures processed level.

For fan or room noise, increase `noiseReductionStrength` gradually. For electrical hum, keep `enableHumNotch` and `enableHumHarmonics` enabled. For keyboard noise, raise the gate thresholds slightly and use the expander rather than a hard gate.
