#pragma once

#include "micshield/audio/AudioEngine.hpp"
#include "micshield/audio/MiniaudioBackend.hpp"
#include "micshield/config/AppConfig.hpp"
#include "micshield/utils/NonCopyable.hpp"

#include <atomic>
#include <memory>

namespace micshield::app {

class Application : private utils::NonCopyable {
public:
    Application();
    ~Application();

    void initialize(const char* configPath, bool forceForeground = false, bool forceBackground = false);
    void run();
    void shutdown();
    void requestStop() noexcept;
    bool runInBackground() const noexcept { return config_.runtime.runInBackground; }

private:
    config::AppConfig config_;
    std::shared_ptr<audio::MiniaudioBackend> backend_;
    std::unique_ptr<audio::AudioEngine> engine_;
    std::atomic<bool> running_{false};
    std::atomic<bool> initialized_{false};
};

} // namespace micshield::app
