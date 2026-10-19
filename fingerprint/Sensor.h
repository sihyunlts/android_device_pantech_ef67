#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>

namespace ef67 {

struct FingerprintCapture {
    static constexpr size_t kFrameSize = 160 * 160;
    static constexpr size_t kMaxFrames = 5;
    std::array<uint8_t, kFrameSize * kMaxFrames> pixels;
    size_t frameCount = 0;

    ~FingerprintCapture();
    void clear();
    FingerprintCapture();
    FingerprintCapture(const FingerprintCapture&) = delete;
    FingerprintCapture& operator=(const FingerprintCapture&) = delete;
};

// One HAL worker owns capture(); cancel() may run on its control thread.
// The owner must stop submitting captures before destroying the sensor.
class FingerprintSensor {
public:
    FingerprintSensor() = default;
    ~FingerprintSensor();
    int open();
    void shutdown();
    int capture(bool firstAttempt, int timeoutMs, FingerprintCapture& result);
    int cancel();

private:
    FingerprintSensor(const FingerprintSensor&) = delete;
    FingerprintSensor& operator=(const FingerprintSensor&) = delete;
    int readCapture(int timeoutMs, FingerprintCapture& result);
    int fd_ = -1;
    int cancelFd_ = -1;
    bool active_ = false;
    bool canceled_ = false;
    std::mutex captureMutex_;
    std::mutex controlMutex_;
};

} // namespace ef67
