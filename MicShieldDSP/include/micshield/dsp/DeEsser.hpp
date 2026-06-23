#pragma once

#include "micshield/dsp/BiquadFilter.hpp"
#include <cstddef>

namespace micshield::dsp {

class DeEsser {
public:
    void prepare(double sampleRate) noexcept;
    void process(float* samples, size_t count) noexcept;

private:
    BiquadFilter detector_;
    float env_ = 0.0f;
};

} // namespace micshield::dsp
