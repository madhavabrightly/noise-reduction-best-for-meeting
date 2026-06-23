#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "micshield/audio/MiniaudioBackend.hpp"

#include <cstring>
#include <stdexcept>

namespace micshield::audio {

struct MiniaudioBackend::Impl {
    ma_context context{};
    ma_device device{};
    bool contextReady = false;
    bool deviceReady = false;
    CaptureCallback capture;
    OutputCallback output;
    bool monitoring = false;
};

namespace {

void dataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    auto* impl = static_cast<MiniaudioBackend::Impl*>(device->pUserData);
    if (input && impl->capture) {
        impl->capture(static_cast<const float*>(input), frameCount, device->capture.channels);
    }
    if (output) {
        if (impl->monitoring && impl->output) impl->output(static_cast<float*>(output), frameCount, device->playback.channels);
        else std::memset(output, 0, frameCount * device->playback.channels * sizeof(float));
    }
}

} // namespace

MiniaudioBackend::MiniaudioBackend() : impl_(std::make_unique<Impl>()) {
    if (ma_context_init(nullptr, 0, nullptr, &impl_->context) != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize miniaudio context");
    }
    impl_->contextReady = true;
}

MiniaudioBackend::~MiniaudioBackend() {
    close();
    if (impl_->contextReady) ma_context_uninit(&impl_->context);
}

std::vector<AudioDevice> MiniaudioBackend::enumerateInputDevices() {
    ma_device_info* playbackInfos = nullptr;
    ma_uint32 playbackCount = 0;
    ma_device_info* captureInfos = nullptr;
    ma_uint32 captureCount = 0;
    if (ma_context_get_devices(&impl_->context, &playbackInfos, &playbackCount, &captureInfos, &captureCount) != MA_SUCCESS) {
        throw std::runtime_error("Failed to enumerate audio devices");
    }
    std::vector<AudioDevice> result;
    result.reserve(captureCount);
    for (ma_uint32 i = 0; i < captureCount; ++i) {
        result.push_back({captureInfos[i].name, captureInfos[i].isDefault != 0});
    }
    (void)playbackInfos;
    (void)playbackCount;
    return result;
}

std::vector<AudioDevice> MiniaudioBackend::enumerateOutputDevices() {
    ma_device_info* playbackInfos = nullptr;
    ma_uint32 playbackCount = 0;
    ma_device_info* captureInfos = nullptr;
    ma_uint32 captureCount = 0;
    if (ma_context_get_devices(&impl_->context, &playbackInfos, &playbackCount, &captureInfos, &captureCount) != MA_SUCCESS) {
        throw std::runtime_error("Failed to enumerate audio devices");
    }
    std::vector<AudioDevice> result;
    result.reserve(playbackCount);
    for (ma_uint32 i = 0; i < playbackCount; ++i) {
        result.push_back({playbackInfos[i].name, playbackInfos[i].isDefault != 0});
    }
    (void)captureInfos;
    (void)captureCount;
    return result;
}

void MiniaudioBackend::open(const AudioFormat& format,
                            const std::string& inputDeviceName,
                            const std::string& outputDeviceName,
                            bool enableOutputMonitoring,
                            CaptureCallback capture,
                            OutputCallback output) {
    close();
    impl_->capture = std::move(capture);
    impl_->output = std::move(output);
    impl_->monitoring = enableOutputMonitoring;

    ma_device_id selectedInputId{};
    ma_device_id selectedOutputId{};
    ma_device_id* selectedInputPtr = nullptr;
    ma_device_id* selectedOutputPtr = nullptr;
    ma_device_info* playbackInfos = nullptr;
    ma_uint32 playbackCount = 0;
    ma_device_info* captureInfos = nullptr;
    ma_uint32 captureCount = 0;
    if (ma_context_get_devices(&impl_->context, &playbackInfos, &playbackCount, &captureInfos, &captureCount) == MA_SUCCESS) {
        for (ma_uint32 i = 0; i < captureCount; ++i) {
            if (!inputDeviceName.empty() && std::string(captureInfos[i].name).find(inputDeviceName) != std::string::npos) {
                selectedInputId = captureInfos[i].id;
                selectedInputPtr = &selectedInputId;
                break;
            }
        }
        for (ma_uint32 i = 0; i < playbackCount; ++i) {
            if (!outputDeviceName.empty() && std::string(playbackInfos[i].name).find(outputDeviceName) != std::string::npos) {
                selectedOutputId = playbackInfos[i].id;
                selectedOutputPtr = &selectedOutputId;
                break;
            }
        }
    }
    (void)playbackInfos;
    (void)playbackCount;

    ma_device_config cfg = ma_device_config_init(enableOutputMonitoring ? ma_device_type_duplex : ma_device_type_capture);
    cfg.capture.pDeviceID = selectedInputPtr;
    cfg.capture.format = ma_format_f32;
    cfg.capture.channels = format.channels;
    cfg.sampleRate = format.sampleRate;
    cfg.periodSizeInFrames = static_cast<ma_uint32>(format.frameSize);
    cfg.dataCallback = dataCallback;
    cfg.pUserData = impl_.get();
    if (enableOutputMonitoring) {
        cfg.playback.pDeviceID = selectedOutputPtr;
        cfg.playback.format = ma_format_f32;
        cfg.playback.channels = format.channels;
    }

    if (ma_device_init(&impl_->context, &cfg, &impl_->device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to open audio device");
    }
    impl_->deviceReady = true;
}

void MiniaudioBackend::start() {
    if (impl_->deviceReady && ma_device_start(&impl_->device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to start audio device");
    }
}

void MiniaudioBackend::stop() {
    if (impl_->deviceReady) ma_device_stop(&impl_->device);
}

void MiniaudioBackend::close() {
    if (impl_->deviceReady) {
        ma_device_uninit(&impl_->device);
        impl_->deviceReady = false;
    }
}

} // namespace micshield::audio
