#pragma once

#include "micshield/audio/AudioFormat.hpp"
#include "micshield/utils/NonCopyable.hpp"

#include <cstdint>
#include <cstddef>
#include <filesystem>
#include <fstream>

namespace micshield::io {

class WavWriter : private utils::NonCopyable {
public:
    WavWriter() = default;
    ~WavWriter();

    void open(const std::filesystem::path& path, const audio::AudioFormat& format);
    void writeFloat32(const float* samples, size_t count);
    void close();
    bool isOpen() const noexcept { return file_.is_open(); }

private:
    void writeHeader(uint32_t dataBytes);
    std::ofstream file_;
    audio::AudioFormat format_;
    uint32_t dataBytes_ = 0;
};

} // namespace micshield::io
