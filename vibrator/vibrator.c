#include <errno.h>
#include <fcntl.h>
#include <hardware/vibrator.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define VIBRATOR_DEVICE "/sys/class/timed_output/vibrator/enable"

static int vibrator_exists(void)
{
    return access(VIBRATOR_DEVICE, W_OK) == 0;
}

static int sendit(unsigned int timeout_ms)
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

static int vibrator_on(vibrator_device_t *device __attribute__((unused)), unsigned int timeout_ms)
{
    return sendit(timeout_ms);
}

static int vibrator_off(vibrator_device_t *device __attribute__((unused)))
{
    return sendit(0);
}

static int vibrator_close(hw_device_t *device)
{
    free(device);
    return 0;
}

static int vibrator_open_device(const hw_module_t *module, const char *id, hw_device_t **device)
{
    if (strcmp(id, VIBRATOR_DEVICE_ID_MAIN) != 0)
        return -EINVAL;
    if (!vibrator_exists())
        return -ENODEV;
    vibrator_device_t *vibrator = calloc(1, sizeof(*vibrator));
    if (!vibrator)
        return -ENOMEM;
    vibrator->common.tag = HARDWARE_DEVICE_TAG;
    vibrator->common.version = HARDWARE_DEVICE_API_VERSION(1, 0);
    vibrator->common.module = (hw_module_t *)module;
    vibrator->common.close = vibrator_close;
    vibrator->vibrator_on = vibrator_on;
    vibrator->vibrator_off = vibrator_off;
    *device = &vibrator->common;
    return 0;
}

static hw_module_methods_t vibrator_methods = { .open = vibrator_open_device };

hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .module_api_version = VIBRATOR_API_VERSION,
    .hal_api_version = HARDWARE_HAL_API_VERSION,
    .id = VIBRATOR_HARDWARE_MODULE_ID,
    .name = "EF67 vibrator",
    .author = "The LineageOS Project",
    .methods = &vibrator_methods,
};
