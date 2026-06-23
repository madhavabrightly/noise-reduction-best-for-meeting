#pragma once

#include <cstddef>

namespace micshield::dsp {

class Expander {
public:
    void prepare(double sampleRate, float thresholdDb, float ratio, float attackMs, float releaseMs, float minGainDb) noexcept;
    void process(float* samples, size_t count, float frameDb) noexcept;

private:
    float thresholdDb_ = -45.0f;
    float ratio_ = 2.0f;
    float minGain_ = 0.25f;
    float attackCoeff_ = 0.05f;
    float releaseCoeff_ = 0.005f;
    float gain_ = 1.0f;
};

} // namespace micshield::dsp
