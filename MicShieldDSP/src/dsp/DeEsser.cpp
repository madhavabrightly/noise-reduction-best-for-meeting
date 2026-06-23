#include "micshield/dsp/DeEsser.hpp"
#include "micshield/utils/MathUtils.hpp"

#include <cmath>

namespace micshield::dsp {

void DeEsser::prepare(double sampleRate) noexcept {
    detector_.setHighPass(sampleRate, 5000.0, 0.707);
    env_ = 0.0f;
}

void DeEsser::process(float* samples, size_t count) noexcept {
    for (size_t i = 0; i < count; ++i) {
        const float band = std::abs(detector_.processSample(samples[i]));
        env_ = utils::smooth(env_, band, band > env_ ? 0.08f : 0.01f);
        const float over = utils::linearToDb(env_) - (-28.0f);
        const float gain = over > 0.0f ? utils::dbToLinear(-utils::clamp(over * 0.35f, 0.0f, 8.0f)) : 1.0f;
        samples[i] = utils::sanitizeFloat(samples[i] * gain);
    }
}

} // namespace micshield::dsp
