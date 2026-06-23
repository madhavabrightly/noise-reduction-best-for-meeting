#pragma once

namespace micshield::dsp {

class NoiseEstimator {
public:
    void prepare(double sampleRate) noexcept;
    void update(float frameDb, bool speechActive) noexcept;
    float getNoiseFloorDb() const noexcept { return noiseFloorDb_; }

private:
    float noiseFloorDb_ = -80.0f;
};

} // namespace micshield::dsp
