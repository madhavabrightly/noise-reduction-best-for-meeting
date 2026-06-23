#include "micshield/dsp/Compressor.hpp"
#include "micshield/utils/MathUtils.hpp"

namespace micshield::dsp {

void Compressor::prepare(double) noexcept { gain_ = 1.0f; }

void Compressor::process(float* samples, size_t count, float frameDb) noexcept {
    const float threshold = -18.0f;
    const float ratio = 3.0f;
    float targetGainDb = 0.0f;
    if (frameDb > threshold) {
        targetGainDb = -((frameDb - threshold) * (1.0f - 1.0f / ratio));
    }
    const float target = utils::dbToLinear(targetGainDb);
    for (size_t i = 0; i < count; ++i) {
        gain_ = utils::smooth(gain_, target, 0.01f);
        samples[i] = utils::sanitizeFloat(samples[i] * gain_);
    }
}

} // namespace micshield::dsp
