#include "micshield/dsp/VoiceActivityDetector.hpp"
#include "micshield/dsp/RmsMeter.hpp"

namespace micshield::dsp {

void VoiceActivityDetector::prepare(double sampleRate, size_t frameSize) noexcept {
    sampleRate_ = sampleRate;
    frameSize_ = frameSize;
    hangoverFrames_ = static_cast<size_t>((0.18 * sampleRate_) / static_cast<double>(frameSize_));
    hangover_ = 0;
}

bool VoiceActivityDetector::process(const float* samples, size_t count, float noiseFloorDb) noexcept {
    RmsMeter meter;
    const float db = meter.process(samples, count);
    const float open = noiseFloorDb + 10.0f;
    const float close = noiseFloorDb + 6.0f;
    if (db > open || db > -42.0f) {
        speechActive_ = true;
        hangover_ = hangoverFrames_;
    } else if (db < close) {
        if (hangover_ > 0) --hangover_;
        else speechActive_ = false;
    }
    return speechActive_;
}

} // namespace micshield::dsp
