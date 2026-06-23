#include "micshield/dsp/BiquadFilter.hpp"
#include "micshield/utils/MathUtils.hpp"

#include <cmath>

namespace micshield::dsp {
namespace {
constexpr double kPi = 3.14159265358979323846;
}

void BiquadFilter::setHighPass(double sr, double f, double q) {
    const double w0 = 2.0 * kPi * f / sr;
    const double c = std::cos(w0);
    const double alpha = std::sin(w0) / (2.0 * q);
    setCoefficients((1.0 + c) * 0.5, -(1.0 + c), (1.0 + c) * 0.5, 1.0 + alpha, -2.0 * c, 1.0 - alpha);
}

void BiquadFilter::setLowPass(double sr, double f, double q) {
    const double w0 = 2.0 * kPi * f / sr;
    const double c = std::cos(w0);
    const double alpha = std::sin(w0) / (2.0 * q);
    setCoefficients((1.0 - c) * 0.5, 1.0 - c, (1.0 - c) * 0.5, 1.0 + alpha, -2.0 * c, 1.0 - alpha);
}

void BiquadFilter::setNotch(double sr, double f, double q) {
    const double w0 = 2.0 * kPi * f / sr;
    const double c = std::cos(w0);
    const double alpha = std::sin(w0) / (2.0 * q);
    setCoefficients(1.0, -2.0 * c, 1.0, 1.0 + alpha, -2.0 * c, 1.0 - alpha);
}

void BiquadFilter::setCoefficients(double b0, double b1, double b2, double a0, double a1, double a2) noexcept {
    b0_ = b0 / a0;
    b1_ = b1 / a0;
    b2_ = b2 / a0;
    a1_ = a1 / a0;
    a2_ = a2 / a0;
}

float BiquadFilter::processSample(float x) noexcept {
    const double y = b0_ * x + z1_;
    z1_ = b1_ * x - a1_ * y + z2_;
    z2_ = b2_ * x - a2_ * y;
    return utils::sanitizeFloat(static_cast<float>(y));
}

void BiquadFilter::process(float* samples, size_t count) noexcept {
    for (size_t i = 0; i < count; ++i) samples[i] = processSample(samples[i]);
}

void BiquadFilter::reset() noexcept {
    z1_ = 0.0;
    z2_ = 0.0;
}

} // namespace micshield::dsp
