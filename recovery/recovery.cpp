#include "partitions.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

// Keep TWRP's default partition behavior when using its device library hook.
BasePartition* make_partition() {
    return new BasePartition();
}

int vibrate(int timeout_ms) {
    // Match TWRP's duration limit and S0226114's strength/time encoding.
    if (timeout_ms > 10000) timeout_ms = 1000;
    const unsigned int command = timeout_ms > 0
            ? (10U << 16) | static_cast<unsigned int>(timeout_ms) : 0;
    char value[16];
    const int length = snprintf(value, sizeof(value), "%u", command);
    const int fd = open("/sys/class/timed_output/vibrator/enable", O_WRONLY | O_CLOEXEC);
    if (fd < 0) return -errno;
    const ssize_t written = write(fd, value, length);
    const int result = written == length ? 0 : (written < 0 ? -errno : -EIO);
    close(fd);
    return result;
}
