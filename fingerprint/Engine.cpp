#include "Engine.h"
#include "Sensor.h"

#include <cerrno>
#include <dlfcn.h>

namespace ef67 {

FingerprintEngine::~FingerprintEngine() {
    close();
}

void FingerprintEngine::close() {
    if (context_)
        freeContext_(context_);
    context_ = nullptr;
    if (library_)
        dlclose(library_);
    library_ = nullptr;
    freeContext_ = nullptr;
    imageQuality_ = nullptr;
    enroll_ = nullptr;
    verify_ = nullptr;
    cancelEnrollment_ = nullptr;
    setAdaptiveMode_ = nullptr;
    list_ = nullptr;
    remove_ = nullptr;
}

int FingerprintEngine::open() {
    if (library_)
        return -EALREADY;
    library_ = dlopen("/system/vendor/lib/libcsfp_core.so", RTLD_NOW | RTLD_LOCAL);
    if (!library_)
        return -ELIBACC;
    auto supported = reinterpret_cast<int (*)(const char*)>(
            dlsym(library_, "CS_IsSupportedSensor"));
    auto allocate = reinterpret_cast<void* (*)(const char*)>(
            dlsym(library_, "CS_AllocateSensorContext"));
    freeContext_ = reinterpret_cast<void (*)(void*)>(
            dlsym(library_, "CS_FreeSensorContext"));
    imageQuality_ = reinterpret_cast<int (*)(void*, const uint8_t*, int)>(
            dlsym(library_, "CS_GetImageQuality"));
    auto certifyLicense = reinterpret_cast<int (*)(void*, const char*)>(
            dlsym(library_, "CS_CertifyLicense"));
    enroll_ = reinterpret_cast<decltype(enroll_)>(dlsym(library_, "CS_Enrol"));
    verify_ = reinterpret_cast<decltype(verify_)>(dlsym(library_, "CS_Verify"));
    cancelEnrollment_ = reinterpret_cast<decltype(cancelEnrollment_)>(
            dlsym(library_, "CS_CancelEnrollment"));
    setAdaptiveMode_ = reinterpret_cast<decltype(setAdaptiveMode_)>(
            dlsym(library_, "CS_SetAdaptiveMode"));
    list_ = reinterpret_cast<decltype(list_)>(dlsym(library_, "CS_GetEnrolledFingerList"));
    remove_ = reinterpret_cast<decltype(remove_)>(dlsym(library_, "CS_DeleteTemplate"));
    auto getDefaultSecurityLevel = reinterpret_cast<int (*)(void*)>(
            dlsym(library_, "CS_GetDefaultSecurityLevel"));
    auto setSecurityLevel = reinterpret_cast<int (*)(void*, int)>(
            dlsym(library_, "CS_SetSecurityLevel"));
    auto setLeastEnrollmentArea = reinterpret_cast<int (*)(void*, int)>(
            dlsym(library_, "CS_SetLeastEnrollmentAreaSize"));
    if (!supported || !allocate || !freeContext_ || !imageQuality_ ||
            !certifyLicense || !enroll_ || !verify_ || !cancelEnrollment_ ||
            !setAdaptiveMode_ || !getDefaultSecurityLevel || !setSecurityLevel ||
            !setLeastEnrollmentArea || !list_ || !remove_) {
        close();
        return -ELIBBAD;
    }
    // S0226114 JNI selects this exact name, even though the DT says IDEX.
    if (!supported("fpc1021")) {
        close();
        return -ENODEV;
    }
    context_ = allocate("fpc1021");
    if (!context_) {
        close();
        return -ENOMEM;
    }
    int status = certifyLicense(context_, "/system/etc/btp.license");
    if (status < 0) {
        close();
        return status;
    }
    int securityLevel = getDefaultSecurityLevel(context_);
    if (securityLevel < 0) {
        close();
        return securityLevel;
    }
    status = setSecurityLevel(context_, securityLevel);
    if (status >= 0) {
        // FingerprintHandler.DEFAULT_LEVEL is 4; requestEnrol uses 22400 * level.
        status = setLeastEnrollmentArea(context_, 22400 * 4);
    }
    if (status < 0) {
        close();
        return status;
    }
    return 0;
}

int FingerprintEngine::imageQuality(const uint8_t* pixels, size_t bytes) {
    if (!context_)
        return -ENODEV;
    if (!pixels || bytes != FingerprintCapture::kFrameSize)
        return -EINVAL;
    // Preserve stock vendor return codes; they are not Android acquired codes.
    return imageQuality_(context_, pixels, static_cast<int>(bytes));
}

int FingerprintEngine::enroll(const uint8_t* pixels, size_t bytes,
                              int fingerIndex, int tryCount,
                              const char* templatePath) {
    if (!context_)
        return -ENODEV;
    if (!pixels || bytes != FingerprintCapture::kFrameSize ||
            fingerIndex < 0 || fingerIndex > 5 || tryCount < 0 ||
            !templatePath || templatePath[0] != '/')
        return -EINVAL;
    // S0226114 JNI passes a five-word descriptor, not a raw path string.
    TemplateFile file = {0, templatePath, nullptr, 0, 0};
    int status = setAdaptiveMode_(context_, 1, 1);
    if (status < 0)
        return status;
    return enroll_(context_, pixels, static_cast<int>(bytes), fingerIndex,
                   tryCount, &file);
}

int FingerprintEngine::verify(const uint8_t* pixels, size_t bytes,
                              const char* templatePath) {
    if (!context_)
        return -ENODEV;
    if (!pixels || bytes != FingerprintCapture::kFrameSize ||
            !templatePath || templatePath[0] != '/')
        return -EINVAL;
    TemplateFile file = {0, templatePath, nullptr, 0, 0};
    int status = setAdaptiveMode_(context_, 0, 0);
    if (status < 0)
        return status;
    return verify_(context_, pixels, static_cast<int>(bytes), &file);
}

int FingerprintEngine::cancelEnrollment(int fingerIndex, int tryCount) {
    if (!context_)
        return -ENODEV;
    if (fingerIndex < 0 || fingerIndex > 5 || tryCount < 0)
        return -EINVAL;
    return cancelEnrollment_(context_, fingerIndex, tryCount);
}

int FingerprintEngine::list(const char* templatePath, int* fingers, size_t capacity) {
    if (!context_)
        return -ENODEV;
    if (!templatePath || !fingers || !capacity || capacity > 6)
        return -EINVAL;
    TemplateFile file = {0, templatePath, nullptr, 0, 0};
    return list_(context_, &file, fingers, static_cast<int>(capacity));
}

int FingerprintEngine::remove(const char* templatePath, int fingerIndex) {
    if (!context_)
        return -ENODEV;
    if (!templatePath || fingerIndex < 0 || fingerIndex > 5)
        return -EINVAL;
    TemplateFile file = {0, templatePath, nullptr, 0, 0};
    return remove_(context_, fingerIndex, &file);
}

} // namespace ef67
