#pragma once

#include "micshield/logging/Logger.hpp"

#define MS_LOG_APP_INFO(...) ::micshield::logging::Logger::get("app")->info(__VA_ARGS__)
#define MS_LOG_AUDIO_INFO(...) ::micshield::logging::Logger::get("audio")->info(__VA_ARGS__)
#define MS_LOG_AUDIO_WARN(...) ::micshield::logging::Logger::get("audio")->warn(__VA_ARGS__)
#define MS_LOG_DSP_INFO(...) ::micshield::logging::Logger::get("dsp")->info(__VA_ARGS__)
#define MS_LOG_DSP_DEBUG(...) ::micshield::logging::Logger::get("dsp")->debug(__VA_ARGS__)
#define MS_LOG_DEVICE_INFO(...) ::micshield::logging::Logger::get("device")->info(__VA_ARGS__)
#define MS_LOG_PERF_INFO(...) ::micshield::logging::Logger::get("performance")->info(__VA_ARGS__)
#define MS_LOG_ERROR(...) ::micshield::logging::Logger::get("error")->error(__VA_ARGS__)
