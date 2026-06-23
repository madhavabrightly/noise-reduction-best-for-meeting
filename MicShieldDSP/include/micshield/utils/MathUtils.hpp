#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace micshield::utils {

constexpr float kMinDb = -120.0f;
constexpr float kEpsilon = 1.0e-12f;

template <typename T>
constexpr T clamp(T v, T lo, T hi) noexcept {
    return std::clamp(v, lo, hi);
}

inline float dbToLinear(float db) noexcept {
    return std::pow(10.0f, db / 20.0f);
}

inline float linearToDb(float linear) noexcept {
    return 20.0f * std::log10(std::max(std::abs(linear), kEpsilon));
}

inline float smooth(float current, float target, float coefficient) noexcept {
    return current + coefficient * (target - current);
}

inline float sanitizeFloat(float v) noexcept {
    return std::isfinite(v) ? clamp(v, -8.0f, 8.0f) : 0.0f;
}

inline bool isFiniteAudio(float v) noexcept {
    return std::isfinite(v);
}

inline float softKnee(float x, float threshold, float kneeWidth) noexcept {
    const float half = kneeWidth * 0.5f;
    if (x < threshold - half) return 0.0f;
    if (x > threshold + half) return x - threshold;
    const float y = x - threshold + half;
    return (y * y) / (2.0f * kneeWidth);
}

} // namespace micshield::utils
