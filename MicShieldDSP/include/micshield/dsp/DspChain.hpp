#pragma once

#include "micshield/config/AppConfig.hpp"
#include "micshield/dsp/Compressor.hpp"
#include "micshield/dsp/DCBlocker.hpp"
#include "micshield/dsp/DeEsser.hpp"
#include "micshield/dsp/Expander.hpp"
#include "micshield/dsp/HighPassFilter.hpp"
#include "micshield/dsp/Limiter.hpp"
#include "micshield/dsp/NoiseEstimator.hpp"
#include "micshield/dsp/NoiseGate.hpp"
#include "micshield/dsp/NoiseReducer.hpp"
#include "micshield/dsp/NotchFilter.hpp"
#include "micshield/dsp/RmsMeter.hpp"
#include "micshield/dsp/VoiceActivityDetector.hpp"
#include "micshield/metrics/AudioMetrics.hpp"

#include <cstddef>

namespace micshield::dsp {

class DspChain {
public:
    void prepare(const config::DspConfig& config, double sampleRate, size_t frameSize);
    void process(float* samples, size_t count) noexcept;
    metrics::AudioMetrics getMetrics() const noexcept;
    void reset() noexcept;

private:
    config::DspConfig config_;
    DCBlocker dc_;
    HighPassFilter highPass_;
    NotchFilter notch_;
    RmsMeter inputMeter_;
    RmsMeter outputMeter_;
    NoiseEstimator estimator_;
    VoiceActivityDetector vad_;
    NoiseReducer reducer_;
    Expander expander_;
    NoiseGate gate_;
    DeEsser deEsser_;
    Compressor compressor_;
    Limiter limiter_;
    metrics::AudioMetrics metrics_;
};

} // namespace micshield::dsp
