#pragma once

#include <array>
#include <cstddef>

namespace micshield::audio {

constexpr size_t kMaxFrameSamples = 4096;

struct AudioFrame {
    std::array<float, kMaxFrameSamples> samples{};
    size_t count = 0;
};

} // namespace micshield::audio
