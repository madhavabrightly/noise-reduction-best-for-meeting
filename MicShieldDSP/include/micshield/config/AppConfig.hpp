#pragma once

#include <cstddef>
#include <string>

namespace micshield::config {

struct AudioConfig {
    uint32_t sampleRate = 48000;
    size_t frameSize = 480;
    uint32_t channels = 1;
    std::string inputDeviceName;
    std::string outputDeviceName;
    bool enableOutputMonitoring = false;
};

struct RuntimeConfig {
    bool runInBackground = true;
    bool setAsDefaultRecordingDevice = false;
    std::string defaultRecordingDeviceName;
};

struct DspConfig {
    bool enableDcBlocker = true;
    bool enableHighPass = true;
    float highPassHz = 80.0f;
    bool enableHumNotch = true;
    float humFrequencyHz = 50.0f;
    bool enableHumHarmonics = true;
    bool enableNoiseReducer = true;
    float noiseReductionStrength = 0.55f;
    float maxNoiseReductionDb = 18.0f;
    bool enableExpander = true;
    float gateOpenDb = -38.0f;
    float gateCloseDb = -48.0f;
    float attackMs = 5.0f;
    float releaseMs = 120.0f;
    bool enableDeEsser = false;
    bool enableCompressor = false;
    bool enableLimiter = true;
    float limiterCeilingDb = -1.0f;
};

struct RecordingConfig {
    bool recordRaw = true;
    bool recordProcessed = true;
};

struct LoggingConfig {
    std::string level = "info";
    bool console = true;
    bool file = true;
    std::string directory = "runtime/logs";
    size_t maxFileSizeMb = 10;
    size_t maxFiles = 5;
};

struct AppConfig {
    AudioConfig audio;
    RuntimeConfig runtime;
    DspConfig dsp;
    RecordingConfig recording;
    LoggingConfig logging;
};

} // namespace micshield::config
