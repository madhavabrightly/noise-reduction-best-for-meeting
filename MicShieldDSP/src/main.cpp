#include "micshield/app/Application.hpp"
#include "micshield/audio/AudioDeviceManager.hpp"
#include "micshield/audio/MiniaudioBackend.hpp"

#include <atomic>
#include <csignal>
#include <exception>
#include <iostream>
#include <thread>
#include <string>

namespace {
std::atomic<micshield::app::Application*> g_app{nullptr};

void onSignal(int) {
    if (auto* app = g_app.load()) app->requestStop();
}
}

int main(int argc, char** argv) {
    const char* configPath = "config/default_config.json";
    bool listDevices = false;
    bool forceForeground = false;
    bool forceBackground = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--list-devices") listDevices = true;
        else if (arg == "--foreground") forceForeground = true;
        else if (arg == "--background") forceBackground = true;
        else configPath = argv[i];
    }

    try {
        if (listDevices) {
            auto backend = std::make_shared<micshield::audio::MiniaudioBackend>();
            micshield::audio::AudioDeviceManager devices(backend);
            std::cout << "Input devices:\n";
            for (const auto& d : devices.enumerateInputDevices()) {
                std::cout << "  " << (d.isDefault ? "* " : "  ") << d.name << '\n';
            }
            std::cout << "Output devices:\n";
            for (const auto& d : devices.enumerateOutputDevices()) {
                std::cout << "  " << (d.isDefault ? "* " : "  ") << d.name << '\n';
            }
            return 0;
        }

        micshield::app::Application app;
        g_app.store(&app);
        std::signal(SIGINT, onSignal);
        app.initialize(configPath, forceForeground, forceBackground);
        if (!app.runInBackground()) {
            std::thread inputThread([] {
                if (std::cin.get()) {
                    if (auto* app = g_app.load()) app->requestStop();
                }
            });
            inputThread.detach();
        }
        app.run();
        g_app.store(nullptr);
        app.shutdown();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
}
