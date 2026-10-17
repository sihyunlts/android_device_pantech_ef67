/*
 * Copyright (C) 2012 The Android Open Source Project
 * Copyright (c) 2012 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "EF67 PowerHAL"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <hardware/power.h>
#include <log/log.h>

#define INTERACTIVE_BOOSTPULSE "/sys/devices/system/cpu/cpufreq/interactive/boostpulse"
#define NOTIFY_ON_MIGRATE "/dev/cpuctl/cpu.notify_on_migrate"

static void write_value(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY | O_CLOEXEC);
    if (fd < 0) {
        ALOGE("Cannot open %s: %s", path, strerror(errno));
        return;
    }

    size_t length = strlen(value);
    ssize_t written = write(fd, value, length);
    if (written < 0)
        ALOGE("Cannot write %s: %s", path, strerror(errno));
    else if ((size_t)written != length)
        ALOGE("Incomplete write to %s", path);
    close(fd);
}

static void set_interactive(struct power_module *module, int on)
{
    (void)module;
    write_value(NOTIFY_ON_MIGRATE, on ? "1" : "0");
}

static void power_init(struct power_module *module)
{
    set_interactive(module, 1);
}

static void power_hint(struct power_module *module, power_hint_t hint, void *data)
{
    (void)module;
    (void)data;
    switch (hint) {
    case POWER_HINT_INTERACTION:
    case POWER_HINT_CPU_BOOST:
        /* Keep the governor's existing boost frequency and duration. */
        write_value(INTERACTIVE_BOOSTPULSE, "1");
        break;
    default:
        break;
    }
}

static struct hw_module_methods_t power_methods = {
    .open = NULL,
};

struct power_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = POWER_MODULE_API_VERSION_0_2,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = POWER_HARDWARE_MODULE_ID,
        .name = "EF67 Power HAL",
        .methods = &power_methods,
    },
    .init = power_init,
    .setInteractive = set_interactive,
    .powerHint = power_hint,
};
