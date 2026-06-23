#include "micshield/audio/WindowsDefaultDeviceManager.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <Propkeydef.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <Mmdeviceapi.h>
#include <Propidl.h>
#include <combaseapi.h>

#include <memory>
#include <string>
#include <vector>

namespace micshield::audio {
namespace {

struct CoTaskMemDeleter {
    void operator()(void* p) const noexcept { CoTaskMemFree(p); }
};

struct PropVariantGuard {
    PROPVARIANT value{};
    PropVariantGuard() { PropVariantInit(&value); }
    ~PropVariantGuard() { PropVariantClear(&value); }
};

std::wstring widen(const std::string& text) {
    if (text.empty()) return {};
    const int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    std::wstring out(static_cast<size_t>(size - 1), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, out.data(), size);
    return out;
}

std::string narrow(const std::wstring& text) {
    if (text.empty()) return {};
    const int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string out(static_cast<size_t>(size - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, out.data(), size, nullptr, nullptr);
    return out;
}

class ComInit {
public:
    ComInit() : hr_(CoInitializeEx(nullptr, COINIT_MULTITHREADED)) {}
    ~ComInit() {
        if (SUCCEEDED(hr_)) CoUninitialize();
    }
    HRESULT result() const noexcept { return hr_; }

private:
    HRESULT hr_;
};

// Undocumented but widely used Windows policy interface for changing the user's default endpoint.
struct __declspec(uuid("f8679f50-850a-41cf-9c72-430f290290c8")) IPolicyConfig : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(LPCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(LPCWSTR, INT, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(LPCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(LPCWSTR, WAVEFORMATEX*, WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(LPCWSTR, INT, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(LPCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(LPCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(LPCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(LPCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(LPCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(LPCWSTR, ERole) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(LPCWSTR, INT) = 0;
};

struct __declspec(uuid("870af99c-171d-4f9e-af0d-e63df40c2bc9")) CPolicyConfigClient;

template <typename T>
void releaseIf(T* p) {
    if (p) p->Release();
}

} // namespace

bool WindowsDefaultDeviceManager::setDefaultCaptureDeviceByName(const std::string& namePart,
                                                                std::string& selectedName,
                                                                std::string& error) const {
    if (namePart.empty()) {
        error = "default capture device name is empty";
        return false;
    }

    ComInit com;
    if (FAILED(com.result()) && com.result() != RPC_E_CHANGED_MODE) {
        error = "failed to initialize COM";
        return false;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr) || !enumerator) {
        error = "failed to create audio endpoint enumerator";
        return false;
    }

    IMMDeviceCollection* collection = nullptr;
    hr = enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &collection);
    if (FAILED(hr) || !collection) {
        releaseIf(enumerator);
        error = "failed to enumerate recording devices";
        return false;
    }

    const std::wstring wanted = widen(namePart);
    UINT count = 0;
    collection->GetCount(&count);
    std::unique_ptr<wchar_t, CoTaskMemDeleter> matchedId;

    for (UINT i = 0; i < count; ++i) {
        IMMDevice* device = nullptr;
        if (FAILED(collection->Item(i, &device)) || !device) continue;

        IPropertyStore* props = nullptr;
        if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &props)) && props) {
            PropVariantGuard friendlyName;
            if (SUCCEEDED(props->GetValue(PKEY_Device_FriendlyName, &friendlyName.value)) &&
                friendlyName.value.vt == VT_LPWSTR) {
                const std::wstring name = friendlyName.value.pwszVal;
                if (name.find(wanted) != std::wstring::npos) {
                    wchar_t* id = nullptr;
                    if (SUCCEEDED(device->GetId(&id))) {
                        matchedId.reset(id);
                        selectedName = narrow(name);
                    }
                }
            }
            props->Release();
        }
        device->Release();
        if (matchedId) break;
    }

    collection->Release();
    enumerator->Release();

    if (!matchedId) {
        error = "matching recording device was not found";
        return false;
    }

    IPolicyConfig* policy = nullptr;
    hr = CoCreateInstance(__uuidof(CPolicyConfigClient), nullptr, CLSCTX_ALL,
                          __uuidof(IPolicyConfig), reinterpret_cast<void**>(&policy));
    if (FAILED(hr) || !policy) {
        error = "failed to create Windows policy config client";
        return false;
    }

    const wchar_t* id = matchedId.get();
    const bool ok = SUCCEEDED(policy->SetDefaultEndpoint(id, eConsole)) &&
                    SUCCEEDED(policy->SetDefaultEndpoint(id, eCommunications)) &&
                    SUCCEEDED(policy->SetDefaultEndpoint(id, eMultimedia));
    policy->Release();

    if (!ok) {
        error = "Windows rejected changing the default recording device";
        return false;
    }
    return true;
}

} // namespace micshield::audio

#else

namespace micshield::audio {

bool WindowsDefaultDeviceManager::setDefaultCaptureDeviceByName(const std::string&,
                                                                std::string&,
                                                                std::string& error) const {
    error = "default device switching is only implemented on Windows";
    return false;
}

} // namespace micshield::audio

#endif
