#include "micshield/logging/Logger.hpp"

#include <filesystem>
#include <mutex>
#include <vector>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace micshield::logging {
namespace {

std::mutex g_loggerMutex;
std::shared_ptr<spdlog::sinks::sink> g_consoleSink;

spdlog::level::level_enum parseLevel(const std::string& level) {
    if (level == "trace") return spdlog::level::trace;
    if (level == "debug") return spdlog::level::debug;
    if (level == "warn") return spdlog::level::warn;
    if (level == "error") return spdlog::level::err;
    return spdlog::level::info;
}

} // namespace

void Logger::initialize(const config::LoggingConfig& config) {
    std::lock_guard lock(g_loggerMutex);
    spdlog::drop_all();
    g_consoleSink.reset();

    if (config.console) {
        g_consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    }

    const auto level = parseLevel(config.level);
    const auto bytes = config.maxFileSizeMb * 1024u * 1024u;
    if (config.file) std::filesystem::create_directories(config.directory);
    for (const auto& name : {"app", "audio", "dsp", "device", "performance", "error"}) {
        std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;
        if (g_consoleSink) sinks.push_back(g_consoleSink);
        if (config.file) {
            auto path = std::filesystem::path(config.directory) / (std::string(name) + ".log");
            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path.string(), bytes, config.maxFiles));
        }
        auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [tid %t] [%n] %v");
        logger->set_level(level);
        logger->flush_on(spdlog::level::warn);
        spdlog::register_logger(logger);
    }
}

void Logger::shutdown() {
    spdlog::shutdown();
}

std::shared_ptr<spdlog::logger> Logger::get(const std::string& name) {
    if (auto logger = spdlog::get(name)) {
        return logger;
    }
    return spdlog::default_logger();
}

} // namespace micshield::logging
