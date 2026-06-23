#include "micshield/audio/AudioDeviceManager.hpp"

#include <algorithm>
#include <stdexcept>

namespace micshield::audio {

AudioDeviceManager::AudioDeviceManager(std::shared_ptr<IAudioBackend> backend) : backend_(std::move(backend)) {}

std::vector<AudioDevice> AudioDeviceManager::enumerateInputDevices() {
    return backend_->enumerateInputDevices();
}

std::vector<AudioDevice> AudioDeviceManager::enumerateOutputDevices() {
    return backend_->enumerateOutputDevices();
}

AudioDevice AudioDeviceManager::selectInputDevice(const std::string& preferredName) {
    auto devices = enumerateInputDevices();
    if (devices.empty()) {
        throw std::runtime_error("No input audio devices found");
    }
    if (!preferredName.empty()) {
        auto it = std::find_if(devices.begin(), devices.end(), [&](const AudioDevice& d) {
            return d.name.find(preferredName) != std::string::npos;
        });
        if (it != devices.end()) return *it;
    }
    auto def = std::find_if(devices.begin(), devices.end(), [](const AudioDevice& d) { return d.isDefault; });
    return def != devices.end() ? *def : devices.front();
}

AudioDevice AudioDeviceManager::selectOutputDevice(const std::string& preferredName) {
    auto devices = enumerateOutputDevices();
    if (devices.empty()) {
        throw std::runtime_error("No output audio devices found");
    }
    if (!preferredName.empty()) {
        auto it = std::find_if(devices.begin(), devices.end(), [&](const AudioDevice& d) {
            return d.name.find(preferredName) != std::string::npos;
        });
        if (it != devices.end()) return *it;
    }
    auto def = std::find_if(devices.begin(), devices.end(), [](const AudioDevice& d) { return d.isDefault; });
    return def != devices.end() ? *def : devices.front();
}

} // namespace micshield::audio
