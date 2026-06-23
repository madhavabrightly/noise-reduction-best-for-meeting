#pragma once

#include "micshield/config/AppConfig.hpp"

#include <memory>
#include <string>
#include <spdlog/logger.h>

namespace micshield::logging {

class Logger {
public:
    static void initialize(const config::LoggingConfig& config);
    static void shutdown();
    static std::shared_ptr<spdlog::logger> get(const std::string& name);
};

} // namespace micshield::logging
