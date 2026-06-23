#pragma once

#include <atomic>
#include <chrono>

namespace micshield::metrics {

class PerformanceMeter {
public:
    void recordFrame(double milliseconds) noexcept;
    double averageMs() const noexcept;
    double maxMs() const noexcept;
    uint64_t frames() const noexcept;
    bool exceedsBudget(double frameDurationMs) const noexcept;

private:
    std::atomic<uint64_t> frames_{0};
    std::atomic<double> totalMs_{0.0};
    std::atomic<double> maxMs_{0.0};
};

} // namespace micshield::metrics
