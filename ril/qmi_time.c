/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/time.h>
#include <time.h>

/* Stock QMI uses gettimeofday only to construct condition-wait deadlines.
 * Pair this clock with pthread_cond_timedwait_monotonic_np so a NITZ clock
 * adjustment cannot expire an outstanding modem request prematurely.
 */
int ef67_qmi_get_monotonic_time(struct timeval *timeval, void *timezone)
{
    struct timespec now;
    (void)timezone;

    int result = clock_gettime(CLOCK_MONOTONIC, &now);
    if (result == 0) {
        timeval->tv_sec = now.tv_sec;
        timeval->tv_usec = now.tv_nsec / 1000;
    }
    return result;
}
