#include "micshield/dsp/NoiseGate.hpp"
#include "micshield/utils/MathUtils.hpp"

namespace micshield::dsp {

void NoiseGate::prepare(float openDb, float closeDb, float floorDb) noexcept {
    openDb_ = openDb;
    closeDb_ = closeDb;
    floorGain_ = utils::dbToLinear(floorDb);
}

void NoiseGate::process(float* samples, size_t count, float frameDb) noexcept {
    if (frameDb > openDb_) open_ = true;
    if (frameDb < closeDb_) open_ = false;
    const float gain = open_ ? 1.0f : floorGain_;
    for (size_t i = 0; i < count; ++i) samples[i] = utils::sanitizeFloat(samples[i] * gain);
}

} // namespace micshield::dsp
