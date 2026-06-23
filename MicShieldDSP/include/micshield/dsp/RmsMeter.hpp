#pragma once

#include <cstddef>

namespace micshield::dsp {

class RmsMeter {
public:
    float process(const float* samples, size_t count) noexcept;
    float getRms() const noexcept { return rms_; }
    float getDbFs() const noexcept { return dbFs_; }

private:
    float rms_ = 0.0f;
    float dbFs_ = -120.0f;
};

} // namespace micshield::dsp
