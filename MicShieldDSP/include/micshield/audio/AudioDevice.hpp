#pragma once

#include <string>

namespace micshield::audio {

struct AudioDevice {
    std::string name;
    bool isDefault = false;
};

} // namespace micshield::audio
