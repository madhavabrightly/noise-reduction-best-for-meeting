#include "micshield/buffer/AudioRingBuffer.hpp"

namespace micshield::buffer {

AudioRingBuffer::AudioRingBuffer(size_t capacityFrames) : ring_(capacityFrames) {}
void AudioRingBuffer::resize(size_t capacityFrames) { ring_.resize(capacityFrames); }
bool AudioRingBuffer::push(const audio::AudioFrame& frame) noexcept { return ring_.push(frame); }
bool AudioRingBuffer::pop(audio::AudioFrame& frame) noexcept { return ring_.pop(frame); }
size_t AudioRingBuffer::availableRead() const noexcept { return ring_.availableRead(); }
size_t AudioRingBuffer::availableWrite() const noexcept { return ring_.availableWrite(); }
void AudioRingBuffer::reset() noexcept { ring_.reset(); }
uint64_t AudioRingBuffer::overflowCount() const noexcept { return ring_.overflowCount(); }
uint64_t AudioRingBuffer::underflowCount() const noexcept { return ring_.underflowCount(); }

} // namespace micshield::buffer
