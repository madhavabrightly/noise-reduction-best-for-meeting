#include "micshield/app/Application.hpp"
#include "micshield/audio/AudioDeviceManager.hpp"
#include "micshield/audio/WindowsDefaultDeviceManager.hpp"
#include "micshield/config/ConfigLoader.hpp"
#include "micshield/logging/LogMacros.hpp"
#include "micshield/logging/Logger.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace micshield::app {

Application::Application() = default;
Application::~Application() { shutdown(); }

void Application::initialize(const char* configPath, bool forceForeground, bool forceBackground) {
    config_ = config::ConfigLoader{}.load(configPath);
    if (forceForeground) config_.runtime.runInBackground = false;
    if (forceBackground) config_.runtime.runInBackground = true;
    logging::Logger::initialize(config_.logging);
    MS_LOG_APP_INFO("MicShieldDSP starting");
    MS_LOG_APP_INFO("Config loaded from {}", configPath);

    backend_ = std::make_shared<audio::MiniaudioBackend>();
    audio::AudioDeviceManager devices(backend_);
    const auto selected = devices.selectInputDevice(config_.audio.inputDeviceName);
    MS_LOG_DEVICE_INFO("Selected input device: {}", selected.name);
    if (config_.audio.enableOutputMonitoring) {
        const auto output = devices.selectOutputDevice(config_.audio.outputDeviceName);
        MS_LOG_DEVICE_INFO("Selected processed output device: {}", output.name);
    }
    if (!config_.audio.inputDeviceName.empty()) {
        MS_LOG_DEVICE_INFO("USB/headset mode requested with matcher: {}", config_.audio.inputDeviceName);
    } else {
        MS_LOG_DEVICE_INFO("Normal/default microphone mode selected");
    }
    if (config_.runtime.setAsDefaultRecordingDevice) {
        std::string selectedDefault;
        std::string error;
        if (audio::WindowsDefaultDeviceManager{}.setDefaultCaptureDeviceByName(
                config_.runtime.defaultRecordingDeviceName, selectedDefault, error)) {
            MS_LOG_DEVICE_INFO("Set Windows default recording/communications device to: {}", selectedDefault);
            std::cout << "Windows default recording device set to: " << selectedDefault << '\n';
        } else {
            MS_LOG_AUDIO_WARN("Unable to set Windows default recording device: {}", error);
            std::cout << "Could not set Windows default recording device: " << error << '\n';
        }
    }

    engine_ = std::make_unique<audio::AudioEngine>(backend_);
    engine_->initialize(config_);
    initialized_.store(true, std::memory_order_release);
    MS_LOG_DSP_INFO("DSP configured: HP={}Hz hum={}Hz NR={} strength={} limiter={}dB",
                    config_.dsp.highPassHz, config_.dsp.humFrequencyHz,
                    config_.dsp.enableNoiseReducer, config_.dsp.noiseReductionStrength,
                    config_.dsp.limiterCeilingDb);
}

void Application::run() {
    running_.store(true, std::memory_order_release);
    engine_->start();
    std::cout << "MicShieldDSP live processing is running.\n";
    std::cout << "Normal mode: leave audio.inputDeviceName empty. USB headset mode: set it to part of the headset name.\n";
    if (config_.audio.enableOutputMonitoring) {
        std::cout << "Processed output routing is enabled. Other apps must record from the matching virtual/input device.\n";
    }
    if (config_.runtime.setAsDefaultRecordingDevice) {
        std::cout << "Apps that use the Windows default microphone should now receive the cleaned virtual mic.\n";
    }
    if (config_.runtime.runInBackground) {
        std::cout << "Background mode is enabled. Stop it from Task Manager or with Stop-Process -Name MicShieldDSP.\n";
    } else {
        std::cout << "Press Enter to stop.\n";
    }
    while (running_.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Application::requestStop() noexcept {
    running_.store(false, std::memory_order_release);
}

void Application::shutdown() {
    if (!initialized_.exchange(false, std::memory_order_acq_rel)) return;
    running_.store(false, std::memory_order_release);
    if (engine_) engine_->stop();
    MS_LOG_APP_INFO("MicShieldDSP shutdown complete");
    logging::Logger::shutdown();
}

} // namespace micshield::app
