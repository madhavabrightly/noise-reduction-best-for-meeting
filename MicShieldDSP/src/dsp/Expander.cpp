#include "micshield/dsp/Expander.hpp"
#include "micshield/utils/MathUtils.hpp"

#include <cmath>

namespace micshield::dsp {

void Expander::prepare(double sampleRate, float thresholdDb, float ratio, float attackMs, float releaseMs, float minGainDb) noexcept {
    thresholdDb_ = thresholdDb;
    ratio_ = ratio;
    minGain_ = utils::dbToLinear(minGainDb);
    attackCoeff_ = 1.0f - std::exp(-1.0f / static_cast<float>(sampleRate * attackMs * 0.001));
    releaseCoeff_ = 1.0f - std::exp(-1.0f / static_cast<float>(sampleRate * releaseMs * 0.001));
    gain_ = 1.0f;
}

void Expander::process(float* samples, size_t count, float frameDb) noexcept {
    float target = 1.0f;
    if (frameDb < thresholdDb_) {
        const float reductionDb = (thresholdDb_ - frameDb) * (ratio_ - 1.0f) / ratio_;
        target = utils::clamp(utils::dbToLinear(-reductionDb), minGain_, 1.0f);
    }
    const float coeff = target < gain_ ? attackCoeff_ : releaseCoeff_;
    for (size_t i = 0; i < count; ++i) {
        gain_ = utils::smooth(gain_, target, coeff);
        samples[i] = utils::sanitizeFloat(samples[i] * gain_);
    }
}

} // namespace micshield::dsp
