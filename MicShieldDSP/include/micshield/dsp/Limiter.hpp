#pragma once

#include <cstddef>
#include <cstdint>

namespace micshield::dsp {

class Limiter {
public:
    void prepare(float ceilingDb) noexcept;
    void process(float* samples, size_t count) noexcept;
    uint64_t clippedSamples() const noexcept { return clipped_; }

private:
    float ceiling_ = 0.891f;
    uint64_t clipped_ = 0;
};

} // namespace micshield::dsp
