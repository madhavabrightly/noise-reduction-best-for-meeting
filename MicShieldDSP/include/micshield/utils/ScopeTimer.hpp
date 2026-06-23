#pragma once

#include <chrono>
#include <functional>

namespace micshield::utils {

class ScopeTimer {
public:
    using Clock = std::chrono::steady_clock;
    using Callback = std::function<void(double)>;

    explicit ScopeTimer(Callback cb) : callback_(std::move(cb)), start_(Clock::now()) {}
    ~ScopeTimer() {
        if (callback_) {
            const auto elapsed = std::chrono::duration<double, std::milli>(Clock::now() - start_).count();
            callback_(elapsed);
        }
    }

private:
    Callback callback_;
    Clock::time_point start_;
};

} // namespace micshield::utils
