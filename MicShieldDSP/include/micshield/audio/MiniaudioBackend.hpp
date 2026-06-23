#pragma once

#include "micshield/audio/IAudioBackend.hpp"
#include "micshield/utils/NonCopyable.hpp"

#include <memory>

namespace micshield::audio {

class MiniaudioBackend : public IAudioBackend, private utils::NonCopyable {
public:
    struct Impl;

    MiniaudioBackend();
    ~MiniaudioBackend() override;

    std::vector<AudioDevice> enumerateInputDevices() override;
    std::vector<AudioDevice> enumerateOutputDevices() override;
    void open(const AudioFormat& format,
              const std::string& inputDeviceName,
              const std::string& outputDeviceName,
              bool enableOutputMonitoring,
              CaptureCallback capture,
              OutputCallback output) override;
    void start() override;
    void stop() override;
    void close() override;

private:
    std::unique_ptr<Impl> impl_;
};

} // namespace micshield::audio
