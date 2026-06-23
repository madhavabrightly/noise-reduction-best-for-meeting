#pragma once

#include <cstddef>

namespace micshield::dsp {

class VoiceActivityDetector {
public:
    void prepare(double sampleRate, size_t frameSize) noexcept;
    bool process(const float* samples, size_t count, float noiseFloorDb) noexcept;
    bool isSpeechActive() const noexcept { return speechActive_; }

private:
    double sampleRate_ = 48000.0;
    size_t frameSize_ = 480;
    bool speechActive_ = false;
    size_t hangoverFrames_ = 15;
    size_t hangover_ = 0;
};

} // namespace micshield::dsp
