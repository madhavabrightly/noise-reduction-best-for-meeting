#pragma once

#include "micshield/audio/AudioFormat.hpp"
#include "micshield/audio/IAudioBackend.hpp"
#include "micshield/buffer/AudioRingBuffer.hpp"
#include "micshield/config/AppConfig.hpp"
#include "micshield/dsp/DspChain.hpp"
#include "micshield/io/WavWriter.hpp"
#include "micshield/metrics/PerformanceMeter.hpp"
#include "micshield/utils/NonCopyable.hpp"

#include <atomic>
#include <memory>
#include <thread>

namespace micshield::audio {

class AudioEngine : private utils::NonCopyable {
public:
    explicit AudioEngine(std::shared_ptr<IAudioBackend> backend);
    ~AudioEngine();

    void initialize(const config::AppConfig& config);
    void start();
    void stop();
    metrics::AudioMetrics metrics() const noexcept;
    metrics::PerformanceMeter const& performance() const noexcept { return performance_; }

private:
    void onCapture(const float* input, size_t frames, uint32_t channels) noexcept;
    void onOutput(float* output, size_t frames, uint32_t channels) noexcept;
    void processingLoop();

    std::shared_ptr<IAudioBackend> backend_;
    AudioFormat format_;
    config::AppConfig config_;
    buffer::AudioRingBuffer inputRing_{256};
    buffer::AudioRingBuffer outputRing_{256};
    dsp::DspChain dsp_;
    io::WavWriter rawWriter_;
    io::WavWriter processedWriter_;
    metrics::PerformanceMeter performance_;
    std::atomic<bool> running_{false};
    std::thread worker_;
};

} // namespace micshield::audio
