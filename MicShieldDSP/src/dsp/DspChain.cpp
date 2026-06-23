#include "micshield/dsp/DspChain.hpp"
#include "micshield/utils/MathUtils.hpp"

#include <algorithm>
#include <cmath>

namespace micshield::dsp {

void DspChain::prepare(const config::DspConfig& config, double sampleRate, size_t frameSize) {
    config_ = config;
    dc_.prepare(sampleRate);
    highPass_.prepare(sampleRate, config.highPassHz);
    notch_.prepare(sampleRate, config.humFrequencyHz, config.enableHumHarmonics);
    estimator_.prepare(sampleRate);
    vad_.prepare(sampleRate, frameSize);
    reducer_.prepare(sampleRate, frameSize);
    expander_.prepare(sampleRate, config.gateOpenDb, 2.2f, config.attackMs, config.releaseMs, -18.0f);
    gate_.prepare(config.gateOpenDb, config.gateCloseDb, -24.0f);
    deEsser_.prepare(sampleRate);
    compressor_.prepare(sampleRate);
    limiter_.prepare(config.limiterCeilingDb);
    reset();
}

void DspChain::process(float* samples, size_t count) noexcept {
    for (size_t i = 0; i < count; ++i) samples[i] = utils::sanitizeFloat(samples[i]);
    if (config_.enableDcBlocker) dc_.process(samples, count);
    if (config_.enableHighPass) highPass_.process(samples, count);
    if (config_.enableHumNotch) notch_.process(samples, count);

    metrics_.inputRmsDb = inputMeter_.process(samples, count);
    const bool speech = vad_.process(samples, count, estimator_.getNoiseFloorDb());
    estimator_.update(metrics_.inputRmsDb, speech);
    metrics_.noiseFloorDb = estimator_.getNoiseFloorDb();
    metrics_.vadActive = speech;

    if (config_.enableNoiseReducer) {
        NoiseReductionContext c{metrics_.inputRmsDb, metrics_.noiseFloorDb, speech, config_.noiseReductionStrength, config_.maxNoiseReductionDb};
        reducer_.process(samples, count, c);
        metrics_.gainReductionDb = reducer_.gainReductionDb();
    }
    if (config_.enableExpander) expander_.process(samples, count, metrics_.inputRmsDb);
    if (config_.enableDeEsser) deEsser_.process(samples, count);
    if (config_.enableCompressor) compressor_.process(samples, count, metrics_.inputRmsDb);
    if (config_.enableLimiter) limiter_.process(samples, count);

    float peak = 0.0f;
    for (size_t i = 0; i < count; ++i) peak = std::max(peak, std::abs(samples[i]));
    metrics_.peakDb = utils::linearToDb(peak);
    metrics_.outputRmsDb = outputMeter_.process(samples, count);
    metrics_.clippedSampleCount = limiter_.clippedSamples();
}

metrics::AudioMetrics DspChain::getMetrics() const noexcept { return metrics_; }

void DspChain::reset() noexcept {
    dc_.reset();
    highPass_.reset();
    notch_.reset();
    metrics_ = {};
}

} // namespace micshield::dsp
