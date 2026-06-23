#include "micshield/metrics/PerformanceMeter.hpp"

#include <algorithm>

namespace micshield::metrics {

void PerformanceMeter::recordFrame(double milliseconds) noexcept {
    frames_.fetch_add(1, std::memory_order_relaxed);
    double oldTotal = totalMs_.load(std::memory_order_relaxed);
    while (!totalMs_.compare_exchange_weak(oldTotal, oldTotal + milliseconds, std::memory_order_relaxed)) {}
    double oldMax = maxMs_.load(std::memory_order_relaxed);
    while (milliseconds > oldMax && !maxMs_.compare_exchange_weak(oldMax, milliseconds, std::memory_order_relaxed)) {}
}

double PerformanceMeter::averageMs() const noexcept {
    const auto count = frames_.load(std::memory_order_relaxed);
    return count == 0 ? 0.0 : totalMs_.load(std::memory_order_relaxed) / static_cast<double>(count);
}

double PerformanceMeter::maxMs() const noexcept {
    return maxMs_.load(std::memory_order_relaxed);
}

uint64_t PerformanceMeter::frames() const noexcept {
    return frames_.load(std::memory_order_relaxed);
}

bool PerformanceMeter::exceedsBudget(double frameDurationMs) const noexcept {
    return maxMs() > frameDurationMs;
}

} // namespace micshield::metrics
