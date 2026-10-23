/* EF67 legacy fingerprint HAL. Enrollment/matching run outside the TEE.
 * Like the klte CM13 compatibility HAL, authentication tokens are not signed.
 * This does not provide Android 6 hardware-backed biometric security.
 */
#include "Engine.h"
#include "Sensor.h"

#include <hardware/hardware.h>
#include <hardware/fingerprint.h>
#include <openssl/sha.h>
#include <endian.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <new>
#include <string>
#include <thread>
#include <vector>

namespace {
constexpr int kFingerCount = 5;
// Stock EnrollTestActivity.handleEnrolStep passes tryCount 0.
constexpr int kEnrollmentTry = 0;
using Clock = std::chrono::steady_clock;

uint64_t nowMs() {
    timespec time;
    if (clock_gettime(CLOCK_BOOTTIME, &time)) return 0;
    return uint64_t(time.tv_sec) * 1000 + time.tv_nsec / 1000000;
}
uint64_t randomId() {
    uint64_t value = 0;
    int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    if (fd < 0) return 0;
    ssize_t count;
    do { count = read(fd, &value, sizeof(value)); } while (count < 0 && errno == EINTR);
    close(fd);
    return count == sizeof(value) ? value : 0;
}
int copyTemplate(const std::string& source, const std::string& target) {
    int input = open(source.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    if (input < 0) return errno == ENOENT ? 0 : -errno;
    int output = open(target.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
    if (output < 0) { int status = -errno; close(input); return status; }
    int status = 0;
    uint8_t buffer[4096];
    for (;;) {
        ssize_t count = read(input, buffer, sizeof(buffer));
        if (count < 0 && errno == EINTR) continue;
        if (count < 0) { status = -errno; break; }
        if (!count) break;
        ssize_t done = 0;
        while (done < count) {
            ssize_t written = write(output, buffer + done, count - done);
            if (written < 0 && errno == EINTR) continue;
            if (written <= 0) { status = written < 0 ? -errno : -EIO; break; }
            done += written;
        }
        if (status) break;
    }
    volatile uint8_t* wipe = buffer;
    for (size_t i = 0; i < sizeof(buffer); ++i) wipe[i] = 0;
    if (!status && fsync(output)) status = -errno;
    close(input); close(output);
    return status;
}
uint64_t templateId(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    if (fd < 0) return 0;
    SHA256_CTX context;
    SHA256_Init(&context);
    uint8_t buffer[4096], hash[SHA256_DIGEST_LENGTH];
    bool valid = true;
    for (;;) {
        ssize_t count = read(fd, buffer, sizeof(buffer));
        if (count < 0 && errno == EINTR) continue;
        if (count < 0) { valid = false; break; }
        if (!count) break;
        SHA256_Update(&context, buffer, count);
    }
    close(fd);
    SHA256_Final(hash, &context);
    volatile uint8_t* wipe = buffer;
    for (size_t i = 0; i < sizeof(buffer); ++i) wipe[i] = 0;
    uint64_t id = 0;
    memcpy(&id, hash, sizeof(id));
    return valid ? (id ? id : 1) : 0;
}

class Controller {
public:
    explicit Controller(fingerprint_device_t& device) : device_(device) {}
    ~Controller() { stop(); }
    ef67::FingerprintEngine engine;
    std::mutex calls;
    std::string directory;
    uint32_t group = 0;
    uint64_t challenge = 0, challengeTime = 0;

    std::string templates() const { return directory + "/templates.bir"; }
    std::string staging() const { return directory + "/enrolling.bir"; }
    void notify(const fingerprint_msg_t& message) { if (device_.notify) device_.notify(&message); }
    void error(fingerprint_error_t code) {
        fingerprint_msg_t message;
        memset(&message, 0, sizeof(message));
        message.type = FINGERPRINT_ERROR; message.data.error = code; notify(message);
    }
    void acquired(fingerprint_acquired_info_t code) {
        fingerprint_msg_t message;
        memset(&message, 0, sizeof(message));
        message.type = FINGERPRINT_ACQUIRED; message.data.acquired.acquired_info = code; notify(message);
    }
    int list(std::vector<int>& fingers) {
        struct stat info;
        if (lstat(templates().c_str(), &info)) return errno == ENOENT ? 0 : -errno;
        if (!S_ISREG(info.st_mode)) return -EINVAL;
        int indices[6] = {-1, -1, -1, -1, -1, -1};
        int status = engine.list(templates().c_str(), indices, 6);
        if (status < 0) return status;
        for (int index : indices) {
            if (index == -1) continue;
            if (index < 0 || index >= kFingerCount ||
                    std::find(fingers.begin(), fingers.end(), index) != fingers.end()) return -EIO;
            fingers.push_back(index);
        }
        return 0;
    }
    uint64_t userId() const {
        uint64_t id = 0;
        int fd = open((directory + "/user_id").c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
        if (fd < 0) return 0;
        ssize_t count = read(fd, &id, sizeof(id)); close(fd);
        return count == sizeof(id) ? id : 0;
    }
    int storeUserId(uint64_t id) {
        const std::string temporary = directory + "/user_id.tmp";
        int fd = open(temporary.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW | O_CLOEXEC, 0600);
        if (fd < 0) return -errno;
        int status = write(fd, &id, sizeof(id)) == sizeof(id) ? 0 : -EIO;
        if (!status && fsync(fd)) status = -errno;
        close(fd);
        if (!status && rename(temporary.c_str(), (directory + "/user_id").c_str())) status = -errno;
        if (status) unlink(temporary.c_str());
        return status;
    }
    void stop() {
        canceled_ = true;
        std::unique_lock<std::mutex> lock(completion_);
        while (running_) {
            lock.unlock(); sensor_.cancel(); lock.lock();
            finished_.wait_for(lock, std::chrono::milliseconds(25), [this] { return !running_; });
        }
        lock.unlock();
        if (worker_.joinable()) worker_.join();
    }
    int start(bool enrolling, int index, uint32_t timeout) {
        if (directory.empty() || !device_.notify) return -EINVAL;
        canceled_ = false;
        enrollmentCommitted_ = false;
        { std::lock_guard<std::mutex> lock(completion_); running_ = true; }
        worker_ = std::thread([this, enrolling, index, timeout] {
            run(enrolling, index, timeout);
            if (enrolling && !enrollmentCommitted_)
                engine.cancelEnrollment(index, kEnrollmentTry);
            sensor_.shutdown();
            if (enrolling) unlink(staging().c_str());
            { std::lock_guard<std::mutex> lock(completion_); running_ = false; }
            finished_.notify_all();
        });
        return 0;
    }
private:
    void run(bool enrolling, int index, uint32_t timeout) {
        if (sensor_.open() < 0) { error(FINGERPRINT_ERROR_HW_UNAVAILABLE); return; }
        if (enrolling && copyTemplate(templates(), staging()) < 0) {
            error(FINGERPRINT_ERROR_UNABLE_TO_PROCESS); return;
        }
        auto deadline = Clock::now() + std::chrono::seconds(timeout);
        int previousRemaining = 100;
        bool first = true;
        while (!canceled_) {
            int captureTimeout = 10000;
            if (enrolling) {
                auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                        deadline - Clock::now()).count();
                if (remaining <= 0) { error(FINGERPRINT_ERROR_TIMEOUT); return; }
                captureTimeout = static_cast<int>(std::min<int64_t>(remaining, captureTimeout));
            }
            ef67::FingerprintCapture image;
            int status = sensor_.capture(first, captureTimeout, image);
            if (canceled_) break;
            if (status == -ETIMEDOUT) continue;
            if (status < 0) { error(FINGERPRINT_ERROR_HW_UNAVAILABLE); return; }
            first = false;
            size_t best = 0;
            int quality = -1;
            for (size_t i = 0; i < image.frameCount; ++i) {
                int value = engine.imageQuality(image.pixels.data() + i * image.kFrameSize, image.kFrameSize);
                if (value > quality) { quality = value; best = i; }
            }
            if (quality <= 0) { acquired(FINGERPRINT_ACQUIRED_INSUFFICIENT); continue; }
            const uint8_t* pixels = image.pixels.data() + best * image.kFrameSize;
            if (enrolling) {
                status = engine.enroll(pixels, image.kFrameSize, index, kEnrollmentTry, staging().c_str());
                image.clear();
                if (canceled_) break;
                if (status < 0) { error(FINGERPRINT_ERROR_UNABLE_TO_PROCESS); return; }
                if (status > 100) { error(FINGERPRINT_ERROR_UNABLE_TO_PROCESS); return; }
                int left = std::min(previousRemaining, 100 - status);
                if (!left) {
                    if (chmod(staging().c_str(), 0600) || rename(staging().c_str(), templates().c_str())) {
                        error(FINGERPRINT_ERROR_UNABLE_TO_PROCESS); return;
                    }
                }
                if (!left) enrollmentCommitted_ = true;
                acquired(FINGERPRINT_ACQUIRED_GOOD);
                fingerprint_msg_t message;
                memset(&message, 0, sizeof(message));
                message.type = FINGERPRINT_TEMPLATE_ENROLLING;
                message.data.enroll.finger = {group, static_cast<uint32_t>(index + 1)};
                message.data.enroll.samples_remaining = left;
                notify(message); previousRemaining = left;
                if (!left) return;
            } else {
                status = engine.verify(pixels, image.kFrameSize, templates().c_str());
                image.clear();
                if (canceled_) break;
                // -60006 is the stock no-match result, confirmed against JNI.
                if (status < 0 && status != -60006) { error(FINGERPRINT_ERROR_UNABLE_TO_PROCESS); return; }
                if (status >= kFingerCount) { error(FINGERPRINT_ERROR_UNABLE_TO_PROCESS); return; }
                acquired(FINGERPRINT_ACQUIRED_GOOD);
                fingerprint_msg_t message;
                memset(&message, 0, sizeof(message));
                message.type = FINGERPRINT_AUTHENTICATED;
                message.data.authenticated.finger = {group, status < 0 ? 0U : static_cast<uint32_t>(status + 1)};
                if (status >= 0) {
                    // Legacy software compatibility, not a TEE-signed authentication token.
                    hw_auth_token_t& token = message.data.authenticated.hat;
                    token.version = HW_AUTH_TOKEN_VERSION;
                    token.user_id = userId();
                    token.authenticator_id = templateId(templates());
                    token.authenticator_type = htobe32(HW_AUTH_FINGERPRINT);
                    token.timestamp = htobe64(nowMs());
                }
                notify(message);
                // Framework owns retry limiting and cancels after its lockout limit.
                if (status >= 0) return;
            }
        }
        // CM13 FingerprintService sends CANCELED itself. Like klte's HAL,
        // finish silently so a duplicate event cannot cancel the next client.
    }
    fingerprint_device_t& device_;
    ef67::FingerprintSensor sensor_;
    std::atomic<bool> canceled_{false};
    std::mutex completion_;
    std::condition_variable finished_;
    bool running_ = false;
    bool enrollmentCommitted_ = false;
    std::thread worker_;
};
struct HalDevice { fingerprint_device_t device; Controller* controller; };
Controller& controller(fingerprint_device_t* device) { return *reinterpret_cast<HalDevice*>(device)->controller; }

int setNotify(fingerprint_device_t* device, fingerprint_notify_t notify) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop(); device->notify = notify; return 0;
}
uint64_t preEnroll(fingerprint_device_t* device) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop(); state.challenge = randomId(); state.challengeTime = nowMs(); return state.challenge;
}
int postEnroll(fingerprint_device_t* device) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop(); state.challenge = 0; return 0;
}
uint64_t getAuthId(fingerprint_device_t* device) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    return state.directory.empty() ? 0 : templateId(state.templates());
}
int cancel(fingerprint_device_t* device) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop(); return 0;
}
int setGroup(fingerprint_device_t* device, uint32_t gid, const char* path) {
    if (!path) return -EINVAL;
    char expected[96]; snprintf(expected, sizeof(expected), "/data/system/users/%u/fpdata", gid);
    if (strcmp(path, expected)) return -EINVAL;
    struct stat info;
    if (lstat(path, &info)) return -errno;
    if (!S_ISDIR(info.st_mode) || info.st_uid != getuid() || (info.st_mode & 0077)) return -EPERM;
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop();
    if (state.directory != path || state.group != gid) state.challenge = 0;
    state.directory = path; state.group = gid;
    unlink(state.staging().c_str());
    return 0;
}
int enroll(fingerprint_device_t* device, const hw_auth_token_t* token, uint32_t gid, uint32_t timeout) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop();
    if (state.directory.empty()) return -EINVAL;
    // No TEE signature check is available. Credential confirmation is supplied
    // by the privileged framework; require its fresh password challenge and SID.
    if (!token || token->version != HW_AUTH_TOKEN_VERSION || !state.challenge ||
            token->challenge != state.challenge || !token->user_id || gid != state.group ||
            !(be32toh(token->authenticator_type) & HW_AUTH_PASSWORD) ||
            nowMs() - state.challengeTime > 300000 || !timeout || timeout > 300) return -EPERM;
    std::vector<int> fingers;
    int status = state.list(fingers); if (status < 0) return status;
    if (!fingers.empty() && state.userId() != token->user_id) return -EPERM;
    int index = 0;
    while (std::find(fingers.begin(), fingers.end(), index) != fingers.end()) ++index;
    if (index >= kFingerCount) { state.error(FINGERPRINT_ERROR_NO_SPACE); return -ENOSPC; }
    status = state.storeUserId(token->user_id); if (status < 0) return status;
    return state.start(true, index, timeout);
}
int authenticate(fingerprint_device_t* device, uint64_t operation, uint32_t gid) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop();
    // Do not claim cryptographic-operation authorization without a valid signature.
    if (operation) return -EOPNOTSUPP;
    if (gid != state.group || state.directory.empty() || !state.userId() ||
            !templateId(state.templates())) return -EINVAL;
    return state.start(false, 0, 0);
}
int enumerate(fingerprint_device_t* device) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop(); if (state.directory.empty()) return -EINVAL;
    std::vector<int> fingers; int status = state.list(fingers); if (status < 0) return status;
    fingerprint_msg_t message = {};
    message.type = FINGERPRINT_TEMPLATE_ENUMERATING;
    message.data.enumerated.finger.gid = state.group;
    if (fingers.empty()) {
        state.notify(message);
        return 0;
    }
    for (size_t i = 0; i < fingers.size(); ++i) {
        message.data.enumerated.finger.fid = static_cast<uint32_t>(fingers[i] + 1);
        message.data.enumerated.remaining_templates = fingers.size() - i - 1;
        state.notify(message);
    }
    return 0;
}
int remove(fingerprint_device_t* device, uint32_t gid, uint32_t fid) {
    auto& state = controller(device); std::lock_guard<std::mutex> lock(state.calls);
    state.stop(); if (gid != state.group || fid > kFingerCount || state.directory.empty()) return -EINVAL;
    std::vector<int> fingers; int status = state.list(fingers); if (status < 0) return status;
    if (fid && std::find(fingers.begin(), fingers.end(), static_cast<int>(fid - 1)) == fingers.end()) return -ENOENT;
    size_t remaining = fid ? 1 : fingers.size();
    for (int index : fingers) {
        if (fid && index + 1 != static_cast<int>(fid)) continue;
        status = state.engine.remove(state.templates().c_str(), index);
        if (status < 0) { state.error(FINGERPRINT_ERROR_UNABLE_TO_REMOVE); return status; }
        fingerprint_msg_t message;
        memset(&message, 0, sizeof(message));
        message.type = FINGERPRINT_TEMPLATE_REMOVED;
        message.data.removed.finger = {gid, static_cast<uint32_t>(index + 1)};
        message.data.removed.remaining_templates = --remaining;
        state.notify(message);
    }
    if (!fid || fingers.size() == 1) {
        unlink(state.templates().c_str()); unlink((state.directory + "/user_id").c_str());
    }
    if (!fid && fingers.empty()) {
        fingerprint_msg_t message;
        memset(&message, 0, sizeof(message)); message.type = FINGERPRINT_TEMPLATE_REMOVED;
        message.data.removed.finger = {gid, 0}; state.notify(message);
    }
    return 0;
}
int closeDevice(hw_device_t* device) {
    auto* hal = reinterpret_cast<HalDevice*>(device);
    delete hal->controller; delete hal; return 0;
}
int openDevice(const hw_module_t* module, const char*, hw_device_t** output) {
    if (!output) return -EINVAL;
    *output = nullptr;
    umask(0077);
    auto* hal = new (std::nothrow) HalDevice;
    if (!hal) return -ENOMEM;
    memset(&hal->device, 0, sizeof(hal->device));
    hal->controller = new (std::nothrow) Controller(hal->device);
    if (!hal->controller) { delete hal; return -ENOMEM; }
    int status = hal->controller->engine.open();
    if (status < 0) { delete hal->controller; delete hal; return status; }
    auto& dev = hal->device;
    dev.common.tag = HARDWARE_DEVICE_TAG; dev.common.version = FINGERPRINT_MODULE_API_VERSION_2_1;
    dev.common.module = const_cast<hw_module_t*>(module); dev.common.close = closeDevice;
    dev.set_notify = setNotify; dev.pre_enroll = preEnroll; dev.enroll = enroll; dev.post_enroll = postEnroll;
    dev.get_authenticator_id = getAuthId; dev.cancel = cancel; dev.enumerate = enumerate;
    dev.remove = remove; dev.set_active_group = setGroup; dev.authenticate = authenticate;
    *output = &dev.common; return 0;
}
hw_module_methods_t methods = {openDevice};
}
extern "C" {
fingerprint_module_t HAL_MODULE_INFO_SYM = {{
    HARDWARE_MODULE_TAG, FINGERPRINT_MODULE_API_VERSION_2_1, HARDWARE_HAL_API_VERSION,
    FINGERPRINT_HARDWARE_MODULE_ID, "EF67 legacy fingerprint HAL", "Pantech / LineageOS",
    &methods, nullptr, {0}
}};
}
