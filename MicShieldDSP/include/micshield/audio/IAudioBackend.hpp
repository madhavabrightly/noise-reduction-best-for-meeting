#pragma once

#include "micshield/audio/AudioDevice.hpp"
#include "micshield/audio/AudioFormat.hpp"
#include "micshield/audio/AudioFrame.hpp"

#include <functional>
#include <vector>

namespace micshield::audio {

using CaptureCallback = std::function<void(const float* input, size_t frames, uint32_t channels)>;
using OutputCallback = std::function<void(float* output, size_t frames, uint32_t channels)>;

class IAudioBackend {
public:
    virtual ~IAudioBackend() = default;
    virtual std::vector<AudioDevice> enumerateInputDevices() = 0;
    virtual std::vector<AudioDevice> enumerateOutputDevices() = 0;
    virtual void open(const AudioFormat& format,
                      const std::string& inputDeviceName,
                      const std::string& outputDeviceName,
                      bool enableOutputMonitoring,
                      CaptureCallback capture,
                      OutputCallback output) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void close() = 0;
};

} // namespace micshield::audio
