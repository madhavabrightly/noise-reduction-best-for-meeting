#pragma once

#include "micshield/dsp/BiquadFilter.hpp"
#include <array>

namespace micshield::dsp {

class NotchFilter {
public:
    void prepare(double sampleRate, double humHz, bool harmonics);
    void reset() noexcept;
    void process(float* samples, size_t count) noexcept;

private:
    std::array<BiquadFilter, 3> filters_;
    size_t active_ = 1;
};

} // namespace micshield::dsp
