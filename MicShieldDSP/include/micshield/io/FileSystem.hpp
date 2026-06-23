#pragma once

#include <filesystem>
#include <string>

namespace micshield::io {

std::filesystem::path ensureDirectory(const std::filesystem::path& path);
std::string timestamp();

} // namespace micshield::io
