#include "micshield/io/WavWriter.hpp"
#include "micshield/io/FileSystem.hpp"

#include <algorithm>

namespace micshield::io {
namespace {

void writeU16(std::ofstream& f, uint16_t v) { f.write(reinterpret_cast<const char*>(&v), sizeof(v)); }
void writeU32(std::ofstream& f, uint32_t v) { f.write(reinterpret_cast<const char*>(&v), sizeof(v)); }

} // namespace

WavWriter::~WavWriter() { close(); }

void WavWriter::open(const std::filesystem::path& path, const audio::AudioFormat& format) {
    close();
    ensureDirectory(path.parent_path());
    format_ = format;
    dataBytes_ = 0;
    file_.open(path, std::ios::binary);
    writeHeader(0);
}

void WavWriter::writeFloat32(const float* samples, size_t count) {
    if (!file_) return;
    file_.write(reinterpret_cast<const char*>(samples), static_cast<std::streamsize>(count * sizeof(float)));
    dataBytes_ += static_cast<uint32_t>(count * sizeof(float));
}

void WavWriter::close() {
    if (!file_) return;
    file_.seekp(0, std::ios::beg);
    writeHeader(dataBytes_);
    file_.close();
}

void WavWriter::writeHeader(uint32_t dataBytes) {
    const uint16_t audioFormat = 3;
    const uint16_t bitsPerSample = 32;
    const uint16_t blockAlign = static_cast<uint16_t>(format_.channels * sizeof(float));
    const uint32_t byteRate = format_.sampleRate * blockAlign;
    file_.write("RIFF", 4);
    writeU32(file_, 36 + dataBytes);
    file_.write("WAVE", 4);
    file_.write("fmt ", 4);
    writeU32(file_, 16);
    writeU16(file_, audioFormat);
    writeU16(file_, static_cast<uint16_t>(format_.channels));
    writeU32(file_, format_.sampleRate);
    writeU32(file_, byteRate);
    writeU16(file_, blockAlign);
    writeU16(file_, bitsPerSample);
    file_.write("data", 4);
    writeU32(file_, dataBytes);
}

} // namespace micshield::io
