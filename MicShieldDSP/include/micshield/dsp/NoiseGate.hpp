#pragma once

#include <cstddef>

namespace micshield::dsp {

class NoiseGate {
public:
    void prepare(float openDb, float closeDb, float floorDb) noexcept;
    void process(float* samples, size_t count, float frameDb) noexcept;

private:
    float openDb_ = -38.0f;
    float closeDb_ = -48.0f;
    float floorGain_ = 0.1f;
    bool open_ = true;
};

} // namespace micshield::dsp
