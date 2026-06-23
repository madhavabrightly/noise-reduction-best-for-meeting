#include "micshield/dsp/DCBlocker.hpp"
#include "micshield/utils/MathUtils.hpp"

namespace micshield::dsp {

void DCBlocker::prepare(double sampleRate) noexcept {
    r_ = sampleRate >= 48000.0 ? 0.995f : 0.990f;
}

void DCBlocker::reset() noexcept {
    x1_ = 0.0f;
    y1_ = 0.0f;
}

void DCBlocker::process(float* samples, size_t count) noexcept {
    for (size_t i = 0; i < count; ++i) {
        const float x = utils::sanitizeFloat(samples[i]);
        const float y = x - x1_ + r_ * y1_;
        x1_ = x;
        y1_ = y;
        samples[i] = utils::sanitizeFloat(y);
    }
}

} // namespace micshield::dsp
