#pragma once

namespace micshield::dsp {

struct NoiseReductionContext {
    float inputDb = -120.0f;
    float noiseFloorDb = -80.0f;
    bool speechActive = false;
    float strength = 0.55f;
    float maxReductionDb = 18.0f;
};

} // namespace micshield::dsp
