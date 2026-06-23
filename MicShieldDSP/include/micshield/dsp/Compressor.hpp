#pragma once

#include <cstddef>

namespace micshield::dsp {

class Compressor {
public:
    void prepare(double sampleRate) noexcept;
    void process(float* samples, size_t count, float frameDb) noexcept;

private:
    float gain_ = 1.0f;
};

} // namespace micshield::dsp
