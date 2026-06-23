#include "micshield/dsp/NotchFilter.hpp"

namespace micshield::dsp {

void NotchFilter::prepare(double sampleRate, double humHz, bool harmonics) {
    active_ = harmonics ? 3 : 1;
    for (size_t i = 0; i < active_; ++i) filters_[i].setNotch(sampleRate, humHz * static_cast<double>(i + 1), 18.0);
}

void NotchFilter::reset() noexcept {
    for (auto& f : filters_) f.reset();
}

void NotchFilter::process(float* samples, size_t count) noexcept {
    for (size_t i = 0; i < active_; ++i) filters_[i].process(samples, count);
}

} // namespace micshield::dsp
