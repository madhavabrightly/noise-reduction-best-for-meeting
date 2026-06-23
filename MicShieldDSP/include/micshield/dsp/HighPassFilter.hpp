#pragma once

#include "micshield/dsp/BiquadFilter.hpp"

namespace micshield::dsp {

class HighPassFilter {
public:
    void prepare(double sampleRate, double cutoffHz);
    void reset() noexcept;
    void process(float* samples, size_t count) noexcept;

private:
    BiquadFilter filter_;
};

} // namespace micshield::dsp
