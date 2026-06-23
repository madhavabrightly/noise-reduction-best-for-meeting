#pragma once

#include "micshield/audio/AudioDevice.hpp"
#include "micshield/audio/IAudioBackend.hpp"

#include <memory>
#include <vector>

namespace micshield::audio {

class AudioDeviceManager {
public:
    explicit AudioDeviceManager(std::shared_ptr<IAudioBackend> backend);
    std::vector<AudioDevice> enumerateInputDevices();
    std::vector<AudioDevice> enumerateOutputDevices();
    AudioDevice selectInputDevice(const std::string& preferredName);
    AudioDevice selectOutputDevice(const std::string& preferredName);

private:
    std::shared_ptr<IAudioBackend> backend_;
};

} // namespace micshield::audio
