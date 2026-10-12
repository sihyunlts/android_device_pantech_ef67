#define LOG_TAG "ef67-wifi-init"
#include <cutils/log.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAC_FILE "/data/misc/wifi/wlan_mac.bin"
#define FACTORY_FILE "/dev/block/platform/msm_sdcc.1/by-name/rawdata"
/* S0226114 pantech_wifi_mac_backup reads this factory record. */
#define FACTORY_WIFI_OFFSET 0x4f600
#define FACTORY_WIFI_MAGIC 0xe20940a8

static int valid_addresses(unsigned char mac[4][6])
{
    int i, j;
    for (i = 0; i < 4; ++i) {
        unsigned char combined = 0;
        for (j = 0; j < 6; ++j)
            combined |= mac[i][j];
        if (!combined || (mac[i][0] & 1))
            return 0;
        for (j = 0; j < i; ++j)
            if (!memcmp(mac[i], mac[j], 6))
                return 0;
    }
    return 1;
}

static int open_valid_mac_file(void)
{
    unsigned char mac[4][6];
    struct stat status;
    int fd = open(MAC_FILE, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    if (fd < 0)
        return -1;
    if (fstat(fd, &status) || !S_ISREG(status.st_mode) ||
            status.st_size != sizeof(mac) ||
            read(fd, mac, sizeof(mac)) != sizeof(mac) || !valid_addresses(mac)) {
        close(fd);
        return -1;
    }
    return fd;
}

static int restore_factory_addresses(void)
{
    unsigned char record[512], mac[4][6];
    char temporary_file[] = MAC_FILE ".XXXXXX";
    uint32_t magic;
    int fd = open(FACTORY_FILE, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        ALOGE("Cannot open factory Wi-Fi record: %s", strerror(errno));
        return -1;
    }
    ssize_t bytes = pread(fd, record, sizeof(record), FACTORY_WIFI_OFFSET);
    close(fd);
    if (bytes != sizeof(record))
        return -1;
    memcpy(&magic, record, sizeof(magic));
    if (magic != FACTORY_WIFI_MAGIC)
        return -1;
    memcpy(mac[0], record + 4, 6);
    memcpy(mac[1], mac[0], 6);
    memcpy(mac[2], record + 10, 6);
    memcpy(mac[3], mac[2], 6);
    mac[1][0] += 2;
    mac[3][0] += 2;
    if (!valid_addresses(mac))
        return -1;
    fd = mkstemp(temporary_file);
    if (fd < 0)
        return -1;
    int result = write(fd, mac, sizeof(mac)) == sizeof(mac) &&
            fchown(fd, 1010, 1010) == 0 && fchmod(fd, 0660) == 0 &&
            fsync(fd) == 0;
    if (close(fd))
        result = 0;
    if (result && rename(temporary_file, MAC_FILE) == 0)
        return 0;
    unlink(temporary_file);
    return -1;
}

int main(void)
{
    int fd = open_valid_mac_file();
    if (fd < 0) {
        if (restore_factory_addresses()) {
            ALOGE("Valid factory Wi-Fi addresses are unavailable");
            return 1;
        }
        fd = open_valid_mac_file();
        if (fd < 0)
            return 1;
    }
    int result = fchown(fd, 1010, 1010) || fchmod(fd, 0660);
    int saved_errno = errno;
    close(fd);
    if (result) {
        ALOGE("Cannot set Wi-Fi address permissions: %s", strerror(saved_errno));
        return 1;
    }
    return 0;
}
