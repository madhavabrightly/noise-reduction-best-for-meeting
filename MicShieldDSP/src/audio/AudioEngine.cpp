#include "micshield/audio/AudioEngine.hpp"
#include "micshield/io/FileSystem.hpp"
#include "micshield/logging/LogMacros.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>

namespace micshield::audio {

AudioEngine::AudioEngine(std::shared_ptr<IAudioBackend> backend) : backend_(std::move(backend)) {}
AudioEngine::~AudioEngine() { stop(); }

void AudioEngine::initialize(const config::AppConfig& config) {
    config_ = config;
    format_ = {config.audio.sampleRate, config.audio.channels, config.audio.frameSize};
    inputRing_.resize(256);
    outputRing_.resize(256);
    dsp_.prepare(config.dsp, format_.sampleRate, format_.frameSize);

    if (config.recording.recordRaw) {
        rawWriter_.open(std::filesystem::path("runtime/recordings/raw") / ("raw_" + io::timestamp() + ".wav"), format_);
    }
    if (config.recording.recordProcessed) {
        processedWriter_.open(std::filesystem::path("runtime/recordings/processed") / ("processed_" + io::timestamp() + ".wav"), format_);
    }

    backend_->open(format_, config.audio.inputDeviceName, config.audio.outputDeviceName, config.audio.enableOutputMonitoring,
        [this](const float* input, size_t frames, uint32_t channels) { onCapture(input, frames, channels); },
        [this](float* output, size_t frames, uint32_t channels) { onOutput(output, frames, channels); });
}

void AudioEngine::start() {
    running_.store(true, std::memory_order_release);
    worker_ = std::thread(&AudioEngine::processingLoop, this);
    backend_->start();
}

void AudioEngine::stop() {
    if (!running_.exchange(false, std::memory_order_acq_rel)) return;
    backend_->stop();
    if (worker_.joinable()) worker_.join();
    backend_->close();
    rawWriter_.close();
    processedWriter_.close();
}

metrics::AudioMetrics AudioEngine::metrics() const noexcept {
    auto m = dsp_.getMetrics();
    m.ringBufferOverflowCount = inputRing_.overflowCount();
    m.ringBufferUnderflowCount = inputRing_.underflowCount();
    return m;
}

void AudioEngine::onCapture(const float* input, size_t frames, uint32_t channels) noexcept {
    size_t offset = 0;
    while (offset < frames) {
        AudioFrame frame;
        const size_t count = std::min(format_.frameSize, frames - offset);
        frame.count = count;
        for (size_t i = 0; i < count; ++i) {
            float mono = 0.0f;
            for (uint32_t ch = 0; ch < channels; ++ch) mono += input[(offset + i) * channels + ch];
            frame.samples[i] = mono / static_cast<float>(channels);
        }
        inputRing_.push(frame);
        offset += count;
    }
}

void AudioEngine::onOutput(float* output, size_t frames, uint32_t channels) noexcept {
    std::memset(output, 0, frames * channels * sizeof(float));
    size_t offset = 0;
    while (offset < frames) {
        AudioFrame frame;
        if (!outputRing_.pop(frame)) break;
        const size_t count = std::min(frame.count, frames - offset);
        for (size_t i = 0; i < count; ++i) {
            for (uint32_t ch = 0; ch < channels; ++ch) output[(offset + i) * channels + ch] = frame.samples[i];
        }
        offset += count;
    }
}

void AudioEngine::processingLoop() {
    MS_LOG_AUDIO_INFO("Processing thread started");
    auto lastMetrics = std::chrono::steady_clock::now();
    while (running_.load(std::memory_order_acquire)) {
        AudioFrame frame;
        if (!inputRing_.pop(frame)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        if (rawWriter_.isOpen()) rawWriter_.writeFloat32(frame.samples.data(), frame.count);
        const auto start = std::chrono::steady_clock::now();
        dsp_.process(frame.samples.data(), frame.count);
        const auto ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        performance_.recordFrame(ms);
        if (processedWriter_.isOpen()) processedWriter_.writeFloat32(frame.samples.data(), frame.count);
        if (config_.audio.enableOutputMonitoring) outputRing_.push(frame);

        const auto now = std::chrono::steady_clock::now();
        if (now - lastMetrics >= std::chrono::seconds(1)) {
            lastMetrics = now;
            const auto m = metrics();
            MS_LOG_PERF_INFO("in={:.1f}dB out={:.1f}dB noise={:.1f}dB vad={} reduction={:.1f}dB avg={:.3f}ms max={:.3f}ms overflow={}",
                             m.inputRmsDb, m.outputRmsDb, m.noiseFloorDb, m.vadActive, m.gainReductionDb,
                             performance_.averageMs(), performance_.maxMs(), m.ringBufferOverflowCount);
            std::cout << "RMS in " << m.inputRmsDb << " dBFS | out " << m.outputRmsDb
                      << " dBFS | noise " << m.noiseFloorDb << " dBFS | VAD "
                      << (m.vadActive ? "speech" : "noise") << " | reduction "
                      << m.gainReductionDb << " dB\r" << std::flush;
        }
    }
    MS_LOG_AUDIO_INFO("Processing thread stopped");
}

} // namespace micshield::audio
