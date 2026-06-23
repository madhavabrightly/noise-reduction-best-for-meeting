#include "micshield/dsp/NoiseEstimator.hpp"
#include "micshield/utils/MathUtils.hpp"

namespace micshield::dsp {

void NoiseEstimator::prepare(double) noexcept { noiseFloorDb_ = -80.0f; }

void NoiseEstimator::update(float frameDb, bool speechActive) noexcept {
    const float alpha = speechActive ? 0.004f : 0.05f;
    const float target = utils::clamp(frameDb, -100.0f, -20.0f);
    noiseFloorDb_ = utils::smooth(noiseFloorDb_, target, alpha);
}

} // namespace micshield::dsp
