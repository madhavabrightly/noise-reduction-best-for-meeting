#pragma once

#include "micshield/audio/AudioFrame.hpp"
#include "micshield/buffer/SpscRingBuffer.hpp"

namespace micshield::buffer {

class AudioRingBuffer {
public:
    explicit AudioRingBuffer(size_t capacityFrames = 0);
    void resize(size_t capacityFrames);
    bool push(const audio::AudioFrame& frame) noexcept;
    bool pop(audio::AudioFrame& frame) noexcept;
    size_t availableRead() const noexcept;
    size_t availableWrite() const noexcept;
    void reset() noexcept;
    uint64_t overflowCount() const noexcept;
    uint64_t underflowCount() const noexcept;

private:
    SpscRingBuffer<audio::AudioFrame> ring_;
};

} // namespace micshield::buffer
