#pragma once

#include <cstddef>
#include <cstdint>

namespace ef67 {

// Calls are serialized by the owner. Matching results are vendor values,
// not Android authentication tokens.
class FingerprintEngine {
public:
    FingerprintEngine() = default;
    ~FingerprintEngine();
    int open();
    int imageQuality(const uint8_t* pixels, size_t bytes);
    int enroll(const uint8_t* pixels, size_t bytes, int fingerIndex,
               int tryCount, const char* templatePath);
    int verify(const uint8_t* pixels, size_t bytes, const char* templatePath);
    int cancelEnrollment(int fingerIndex, int tryCount);
    int list(const char* templatePath, int* fingers, size_t capacity);
    int remove(const char* templatePath, int fingerIndex);

private:
    FingerprintEngine(const FingerprintEngine&) = delete;
    FingerprintEngine& operator=(const FingerprintEngine&) = delete;
    void close();
    void* library_ = nullptr;
    void* context_ = nullptr;
    void (*freeContext_)(void*) = nullptr;
    struct TemplateFile {
        int type;
        const char* path;
        void* memory;
        int used;
        int capacity;
    };
    int (*imageQuality_)(void*, const uint8_t*, int) = nullptr;
    int (*enroll_)(void*, const uint8_t*, int, int, int, TemplateFile*) = nullptr;
    int (*verify_)(void*, const uint8_t*, int, TemplateFile*) = nullptr;
    int (*cancelEnrollment_)(void*, int, int) = nullptr;
    int (*setAdaptiveMode_)(void*, int, int) = nullptr;
    int (*list_)(void*, TemplateFile*, int*, int) = nullptr;
    int (*remove_)(void*, int, TemplateFile*) = nullptr;
};

} // namespace ef67
