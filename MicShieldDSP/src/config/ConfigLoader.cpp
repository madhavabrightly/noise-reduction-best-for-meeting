#include "micshield/config/ConfigLoader.hpp"
#include "micshield/utils/MathUtils.hpp"

#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace micshield::config {
namespace {

template <typename T>
void readIf(const nlohmann::json& object, const char* key, T& out) {
    if (object.contains(key) && !object.at(key).is_null()) {
        out = object.at(key).get<T>();
    }
}

} // namespace

AppConfig ConfigLoader::load(const std::filesystem::path& path) const {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Unable to open config file: " + path.string());
    }

    nlohmann::json j;
    in >> j;

    AppConfig cfg;
    if (auto it = j.find("audio"); it != j.end()) {
        readIf(*it, "sampleRate", cfg.audio.sampleRate);
        readIf(*it, "frameSize", cfg.audio.frameSize);
        readIf(*it, "channels", cfg.audio.channels);
        readIf(*it, "inputDeviceName", cfg.audio.inputDeviceName);
        readIf(*it, "outputDeviceName", cfg.audio.outputDeviceName);
        readIf(*it, "enableOutputMonitoring", cfg.audio.enableOutputMonitoring);
    }
    if (auto it = j.find("runtime"); it != j.end()) {
        readIf(*it, "runInBackground", cfg.runtime.runInBackground);
        readIf(*it, "setAsDefaultRecordingDevice", cfg.runtime.setAsDefaultRecordingDevice);
        readIf(*it, "defaultRecordingDeviceName", cfg.runtime.defaultRecordingDeviceName);
    }
    if (auto it = j.find("dsp"); it != j.end()) {
        readIf(*it, "enableDcBlocker", cfg.dsp.enableDcBlocker);
        readIf(*it, "enableHighPass", cfg.dsp.enableHighPass);
        readIf(*it, "highPassHz", cfg.dsp.highPassHz);
        readIf(*it, "enableHumNotch", cfg.dsp.enableHumNotch);
        readIf(*it, "humFrequencyHz", cfg.dsp.humFrequencyHz);
        readIf(*it, "enableHumHarmonics", cfg.dsp.enableHumHarmonics);
        readIf(*it, "enableNoiseReducer", cfg.dsp.enableNoiseReducer);
        readIf(*it, "noiseReductionStrength", cfg.dsp.noiseReductionStrength);
        readIf(*it, "maxNoiseReductionDb", cfg.dsp.maxNoiseReductionDb);
        readIf(*it, "enableExpander", cfg.dsp.enableExpander);
        readIf(*it, "gateOpenDb", cfg.dsp.gateOpenDb);
        readIf(*it, "gateCloseDb", cfg.dsp.gateCloseDb);
        readIf(*it, "attackMs", cfg.dsp.attackMs);
        readIf(*it, "releaseMs", cfg.dsp.releaseMs);
        readIf(*it, "enableDeEsser", cfg.dsp.enableDeEsser);
        readIf(*it, "enableCompressor", cfg.dsp.enableCompressor);
        readIf(*it, "enableLimiter", cfg.dsp.enableLimiter);
        readIf(*it, "limiterCeilingDb", cfg.dsp.limiterCeilingDb);
    }
    if (auto it = j.find("recording"); it != j.end()) {
        readIf(*it, "recordRaw", cfg.recording.recordRaw);
        readIf(*it, "recordProcessed", cfg.recording.recordProcessed);
    }
    if (auto it = j.find("logging"); it != j.end()) {
        readIf(*it, "level", cfg.logging.level);
        readIf(*it, "console", cfg.logging.console);
        readIf(*it, "file", cfg.logging.file);
        readIf(*it, "directory", cfg.logging.directory);
        readIf(*it, "maxFileSizeMb", cfg.logging.maxFileSizeMb);
        readIf(*it, "maxFiles", cfg.logging.maxFiles);
    }

    validate(cfg);
    return cfg;
}

void ConfigLoader::validate(AppConfig& cfg) {
    cfg.audio.sampleRate = static_cast<uint32_t>(utils::clamp(cfg.audio.sampleRate, 8000u, 192000u));
    cfg.audio.frameSize = utils::clamp<size_t>(cfg.audio.frameSize, 64, 4096);
    cfg.audio.channels = static_cast<uint32_t>(utils::clamp(cfg.audio.channels, 1u, 2u));
    cfg.dsp.highPassHz = utils::clamp(cfg.dsp.highPassHz, 20.0f, 400.0f);
    cfg.dsp.humFrequencyHz = utils::clamp(cfg.dsp.humFrequencyHz, 45.0f, 65.0f);
    cfg.dsp.noiseReductionStrength = utils::clamp(cfg.dsp.noiseReductionStrength, 0.0f, 1.0f);
    cfg.dsp.maxNoiseReductionDb = utils::clamp(cfg.dsp.maxNoiseReductionDb, 0.0f, 40.0f);
    cfg.dsp.gateOpenDb = utils::clamp(cfg.dsp.gateOpenDb, -90.0f, -5.0f);
    cfg.dsp.gateCloseDb = utils::clamp(cfg.dsp.gateCloseDb, -100.0f, cfg.dsp.gateOpenDb);
    cfg.dsp.attackMs = utils::clamp(cfg.dsp.attackMs, 0.1f, 200.0f);
    cfg.dsp.releaseMs = utils::clamp(cfg.dsp.releaseMs, 1.0f, 2000.0f);
    cfg.dsp.limiterCeilingDb = utils::clamp(cfg.dsp.limiterCeilingDb, -12.0f, -0.1f);
}

} // namespace micshield::config
