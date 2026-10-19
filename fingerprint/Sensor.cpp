#include "Sensor.h"

#include <cerrno>
#include <chrono>
#include <fcntl.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace ef67 {
namespace {
constexpr unsigned long kCapture = _IO(0xfc, 0);
constexpr unsigned long kAbort = _IO(0xfc, 1);
constexpr unsigned long kFirstTry = _IOW(0xfc, 6, unsigned int);
}

FingerprintCapture::FingerprintCapture() {
    clear();
}

void FingerprintCapture::clear() {
    volatile uint8_t* cursor = pixels.data();
    for (size_t i = 0; i < pixels.size(); ++i)
        cursor[i] = 0;
    frameCount = 0;
}

FingerprintCapture::~FingerprintCapture() {
    clear();
}

FingerprintSensor::~FingerprintSensor() {
    shutdown();
}

void FingerprintSensor::shutdown() {
    cancel();
    std::lock_guard<std::mutex> captureLock(captureMutex_);
    std::lock_guard<std::mutex> controlLock(controlMutex_);
    if (fd_ >= 0)
        close(fd_);
    if (cancelFd_ >= 0)
        close(cancelFd_);
    fd_ = cancelFd_ = -1;
}

int FingerprintSensor::open() {
    std::lock_guard<std::mutex> captureLock(captureMutex_);
    if (fd_ >= 0)
        return -EALREADY;
    int fd = ::open("/dev/fpc_btp", O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (fd < 0)
        return -errno;
    int cancelFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (cancelFd < 0) {
        int error = -errno;
        close(fd);
        return error;
    }
    std::lock_guard<std::mutex> controlLock(controlMutex_);
    fd_ = fd;
    cancelFd_ = cancelFd;
    return 0;
}

int FingerprintSensor::cancel() {
    std::lock_guard<std::mutex> controlLock(controlMutex_);
    if (!active_)
        return 0;
    canceled_ = true;
    // Wake the poll before waiting for the kernel worker to stop.
    uint64_t wake = 1;
    ssize_t count;
    do {
        count = write(cancelFd_, &wake, sizeof(wake));
    } while (count < 0 && errno == EINTR);
    return ioctl(fd_, kAbort, 0) < 0 ? -errno : 0;
}

int FingerprintSensor::capture(bool firstAttempt, int timeoutMs,
                               FingerprintCapture& result) {
    std::lock_guard<std::mutex> captureLock(captureMutex_);
    result.clear();
    if (timeoutMs <= 0)
        return -EINVAL;
    {
        std::lock_guard<std::mutex> controlLock(controlMutex_);
        if (fd_ < 0)
            return -EBADF;
        uint64_t wake;
        while (read(cancelFd_, &wake, sizeof(wake)) < 0 && errno == EINTR) {}
        canceled_ = false;
        // This ABI takes the flag value directly, not a pointer.
        if (ioctl(fd_, kFirstTry, firstAttempt ? 1UL : 0UL) < 0)
            return -errno;
        if (ioctl(fd_, kCapture, 0) < 0)
            return -errno;
        active_ = true;
    }
    int status = readCapture(timeoutMs, result);
    {
        std::lock_guard<std::mutex> controlLock(controlMutex_);
        if (canceled_)
            status = -ECANCELED;
        if (status < 0 && !canceled_)
            ioctl(fd_, kAbort, 0);
        active_ = false;
    }
    if (status < 0)
        result.clear();
    return status;
}

int FingerprintSensor::readCapture(int timeoutMs, FingerprintCapture& result) {
    auto deadline = std::chrono::steady_clock::now() +
                    std::chrono::milliseconds(timeoutMs);
    size_t bytes = 0;
    for (;;) {
        auto left = std::chrono::duration_cast<std::chrono::milliseconds>(
                deadline - std::chrono::steady_clock::now()).count();
        if (left <= 0)
            return -ETIMEDOUT;
        pollfd events[2] = {{fd_, POLLIN, 0}, {cancelFd_, POLLIN, 0}};
        int ready = poll(events, 2, static_cast<int>(left));
        if (ready < 0) {
            if (errno == EINTR)
                continue;
            return -errno;
        }
        if (!ready)
            return -ETIMEDOUT;
        if (events[1].revents)
            return -ECANCELED;
        if (events[0].revents & (POLLERR | POLLNVAL))
            return -EIO;
        if (events[0].revents & POLLIN) {
            if (bytes == result.pixels.size())
                return -EOVERFLOW;
            ssize_t count = read(fd_, result.pixels.data() + bytes,
                                 result.pixels.size() - bytes);
            if (count < 0) {
                if (errno == EINTR || errno == EAGAIN)
                    continue;
                return -errno;
            }
            if (count > 0) {
                bytes += static_cast<size_t>(count);
                continue;
            }
        }
        if ((events[0].revents & POLLHUP) || (events[0].revents & POLLIN)) {
            if (!bytes || bytes % FingerprintCapture::kFrameSize)
                return -EIO;
            result.frameCount = bytes / FingerprintCapture::kFrameSize;
            return 0;
        }
    }
}

} // namespace ef67
