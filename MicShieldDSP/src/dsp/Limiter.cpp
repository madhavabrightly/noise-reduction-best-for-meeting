#include "micshield/dsp/Limiter.hpp"
#include "micshield/utils/MathUtils.hpp"

namespace micshield::dsp {

void Limiter::prepare(float ceilingDb) noexcept {
    ceiling_ = utils::dbToLinear(ceilingDb);
    clipped_ = 0;
}

void Limiter::process(float* samples, size_t count) noexcept {
    for (size_t i = 0; i < count; ++i) {
        const float x = utils::sanitizeFloat(samples[i]);
        if (x > ceiling_) { samples[i] = ceiling_; ++clipped_; }
        else if (x < -ceiling_) { samples[i] = -ceiling_; ++clipped_; }
        else samples[i] = x;
    }
}

} // namespace micshield::dsp
