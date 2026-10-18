#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define VIBRATOR_DEVICE "/sys/class/timed_output/vibrator/enable"

int vibrator_exists(void)
{
    return access(VIBRATOR_DEVICE, W_OK) == 0;
}

static int sendit(int timeout_ms)
{
    unsigned int value = 0;
    char text[24];
    int fd, length;
    ssize_t written;

    /* S0226114 vibrator_on uses level 10 and a 16-bit duration. */
    if (timeout_ms > 0) {
        if (timeout_ms > 32767)
            timeout_ms = 32767;
        value = (10U << 16) | (unsigned int)timeout_ms;
    }
    fd = open(VIBRATOR_DEVICE, O_WRONLY | O_CLOEXEC);
    if (fd < 0)
        return -errno;
    length = snprintf(text, sizeof(text), "%u\n", value);
    written = write(fd, text, length);
    close(fd);
    return written == length ? 0 : -1;
}

int vibrator_on(int timeout_ms)
{
    return sendit(timeout_ms);
}

int vibrator_off(void)
{
    return sendit(0);
}
