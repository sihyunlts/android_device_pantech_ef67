/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hardware/lights.h>

static pthread_mutex_t lights_lock = PTHREAD_MUTEX_INITIALIZER;
static struct light_state_t notification_state, battery_state, attention_state;

static int write_value(const char *path, int value)
{
    char buffer[20];
    int fd = open(path, O_WRONLY | O_CLOEXEC);
    if (fd < 0)
        return -errno;
    int length = snprintf(buffer, sizeof(buffer), "%d\n", value);
    ssize_t written = write(fd, buffer, length);
    int result = written < 0 ? -errno : written == length ? 0 : -EIO;
    close(fd);
    return result;
}

static int write_channel(int channel, int value)
{
    char path[80];
    snprintf(path, sizeof(path), "/sys/class/leds/lp5523:channel%d/brightness", channel);
    return write_value(path, value);
}

static int brightness(const struct light_state_t *state)
{
    return (77 * ((state->color >> 16) & 255) +
            150 * ((state->color >> 8) & 255) + 29 * (state->color & 255)) >> 8;
}

static int set_backlight(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    pthread_mutex_lock(&lights_lock);
    int result = write_value("/sys/class/leds/lcd-backlight/brightness", brightness(state));
    pthread_mutex_unlock(&lights_lock);
    return result;
}

static int set_buttons(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    pthread_mutex_lock(&lights_lock);
    int result = write_channel(4, brightness(state));
    if (!result)
        result = write_channel(5, brightness(state));
    pthread_mutex_unlock(&lights_lock);
    return result;
}

static int time_units(int milliseconds, int unit)
{
    int value = milliseconds / unit + (milliseconds % unit >= unit / 2);
    return value < 1 ? 1 : value > 255 ? 255 : value;
}

static int apply_indicator_locked(void)
{
    const struct light_state_t *state = &battery_state;
    if (attention_state.color & 0xffffff)
        state = &attention_state;
    else if (notification_state.color & 0xffffff)
        state = &notification_state;

    /* Command 0 stops RGB engines without changing the touchkey outputs. */
    int result = write_channel(8, 0);
    if (result || !(state->color & 0xffffff))
        return result;

    int values[8] = {
        (state->color >> 8) & 255, state->color & 255, 0, 0, 0, 0,
        (state->color >> 16) & 255, 0
    };
    int command = 1;
    if ((state->flashMode == LIGHT_FLASH_TIMED ||
         state->flashMode == LIGHT_FLASH_HARDWARE) &&
            state->flashOnMS > 0 && state->flashOffMS > 0) {
        /* S0226114: channel 2 uses 10ms, channel 3 uses 100ms. */
        values[2] = time_units(state->flashOnMS, 10);
        values[3] = time_units(state->flashOffMS, 100);
        command = 2;
    }
    const int channels[] = { 0, 1, 2, 3, 6, 7 };
    unsigned int i;
    for (i = 0; i < sizeof(channels) / sizeof(channels[0]); ++i) {
        result = write_channel(channels[i], values[channels[i]]);
        if (result)
            return result;
    }
    return write_channel(8, command);
}

static int set_indicator(struct light_state_t *saved, const struct light_state_t *state)
{
    pthread_mutex_lock(&lights_lock);
    *saved = *state;
    int result = apply_indicator_locked();
    pthread_mutex_unlock(&lights_lock);
    return result;
}

static int set_notifications(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    return set_indicator(&notification_state, state);
}

static int set_battery(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    return set_indicator(&battery_state, state);
}

static int set_attention(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    return set_indicator(&attention_state, state);
}

static int close_light(struct hw_device_t *device)
{
    free(device);
    return 0;
}

static int open_light(const struct hw_module_t *module, const char *name,
                      struct hw_device_t **device)
{
    int (*set_light)(struct light_device_t *, const struct light_state_t *);
    if (!strcmp(name, LIGHT_ID_BACKLIGHT))
        set_light = set_backlight;
    else if (!strcmp(name, LIGHT_ID_BUTTONS))
        set_light = set_buttons;
    else if (!strcmp(name, LIGHT_ID_NOTIFICATIONS))
        set_light = set_notifications;
    else if (!strcmp(name, LIGHT_ID_BATTERY))
        set_light = set_battery;
    else if (!strcmp(name, LIGHT_ID_ATTENTION))
        set_light = set_attention;
    else
        return -EINVAL;
    struct light_device_t *light = calloc(1, sizeof(*light));
    if (!light)
        return -ENOMEM;
    light->common.tag = HARDWARE_DEVICE_TAG;
    light->common.version = 0;
    light->common.module = (struct hw_module_t *)module;
    light->common.close = close_light;
    light->set_light = set_light;
    *device = &light->common;
    return 0;
}

static struct hw_module_methods_t lights_methods = { .open = open_light };
struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "EF67 lights",
    .methods = &lights_methods,
};
