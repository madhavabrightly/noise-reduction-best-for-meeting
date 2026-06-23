#include "micshield/dsp/NoiseReducer.hpp"
#include "micshield/utils/MathUtils.hpp"

namespace micshield::dsp {

void NoiseReducer::prepare(double, size_t) noexcept {
    smoothedGain_ = 1.0f;
    gainReductionDb_ = 0.0f;
}

void NoiseReducer::process(float* samples, size_t count, const NoiseReductionContext& c) noexcept {
    const float margin = c.inputDb - c.noiseFloorDb;
    float reductionDb = 0.0f;
    if (!c.speechActive && margin < 18.0f) {
        reductionDb = c.maxReductionDb * c.strength;
    } else if (margin < 10.0f) {
        reductionDb = c.maxReductionDb * c.strength * 0.45f;
    }
    reductionDb = utils::clamp(reductionDb, 0.0f, c.maxReductionDb);
    const float targetGain = utils::dbToLinear(-reductionDb);
    smoothedGain_ = utils::smooth(smoothedGain_, targetGain, c.speechActive ? 0.08f : 0.18f);
    gainReductionDb_ = -utils::linearToDb(smoothedGain_);
    for (size_t i = 0; i < count; ++i) samples[i] = utils::sanitizeFloat(samples[i] * smoothedGain_);
}

} // namespace micshield::dsp
