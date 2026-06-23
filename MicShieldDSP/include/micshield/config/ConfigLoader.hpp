#pragma once

#include "micshield/config/AppConfig.hpp"

#include <filesystem>

namespace micshield::config {

class ConfigLoader {
public:
    AppConfig load(const std::filesystem::path& path) const;

private:
    static void validate(AppConfig& config);
};

} // namespace micshield::config
