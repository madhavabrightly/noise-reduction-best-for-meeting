#pragma once

#include <cstddef>

namespace micshield::dsp {

class BiquadFilter {
public:
    void setHighPass(double sampleRate, double frequency, double q);
    void setLowPass(double sampleRate, double frequency, double q);
    void setNotch(double sampleRate, double frequency, double q);
    float processSample(float x) noexcept;
    void process(float* samples, size_t count) noexcept;
    void reset() noexcept;

private:
    void setCoefficients(double b0, double b1, double b2, double a0, double a1, double a2) noexcept;
    double b0_ = 1.0, b1_ = 0.0, b2_ = 0.0, a1_ = 0.0, a2_ = 0.0;
    double z1_ = 0.0, z2_ = 0.0;
};

} // namespace micshield::dsp
