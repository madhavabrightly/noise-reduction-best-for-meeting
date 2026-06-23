#pragma once

#include <string>

namespace micshield::audio {

class WindowsDefaultDeviceManager {
public:
    bool setDefaultCaptureDeviceByName(const std::string& namePart, std::string& selectedName, std::string& error) const;
};

} // namespace micshield::audio
