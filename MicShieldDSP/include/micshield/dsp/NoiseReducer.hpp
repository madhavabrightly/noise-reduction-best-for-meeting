#pragma once

#include "micshield/dsp/DspTypes.hpp"
#include <cstddef>

namespace micshield::dsp {

class NoiseReducer {
public:
    void prepare(double sampleRate, size_t frameSize) noexcept;
    void process(float* samples, size_t count, const NoiseReductionContext& context) noexcept;
    float gainReductionDb() const noexcept { return gainReductionDb_; }

private:
    float smoothedGain_ = 1.0f;
    float gainReductionDb_ = 0.0f;
};

} // namespace micshield::dsp
