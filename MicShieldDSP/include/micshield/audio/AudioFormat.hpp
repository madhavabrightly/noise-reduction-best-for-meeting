#pragma once

#include <cstddef>
#include <cstdint>

namespace micshield::audio {

struct AudioFormat {
    uint32_t sampleRate = 48000;
    uint32_t channels = 1;
    size_t frameSize = 480;
};

} // namespace micshield::audio
