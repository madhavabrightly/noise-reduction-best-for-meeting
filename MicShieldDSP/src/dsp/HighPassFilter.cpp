#include "micshield/dsp/HighPassFilter.hpp"

namespace micshield::dsp {

void HighPassFilter::prepare(double sampleRate, double cutoffHz) { filter_.setHighPass(sampleRate, cutoffHz, 0.707); }
void HighPassFilter::reset() noexcept { filter_.reset(); }
void HighPassFilter::process(float* samples, size_t count) noexcept { filter_.process(samples, count); }

} // namespace micshield::dsp
