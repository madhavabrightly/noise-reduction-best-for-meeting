#pragma once

#include <cstddef>

namespace micshield::dsp {

class DCBlocker {
public:
    void prepare(double sampleRate) noexcept;
    void reset() noexcept;
    void process(float* samples, size_t count) noexcept;

private:
    float r_ = 0.995f;
    float x1_ = 0.0f;
    float y1_ = 0.0f;
};

} // namespace micshield::dsp
