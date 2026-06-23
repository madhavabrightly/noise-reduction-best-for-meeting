#include "micshield/io/FileSystem.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace micshield::io {

std::filesystem::path ensureDirectory(const std::filesystem::path& path) {
    std::filesystem::create_directories(path);
    return path;
}

std::string timestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream out;
    out << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return out.str();
}

} // namespace micshield::io
