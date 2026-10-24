/* Read the existing Qualcomm Bluetooth NV address before the HIDL HAL starts. */
#include <cutils/properties.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>

/* ABI used by Qualcomm's bt_vendor_persist.cpp with BT_NV_SUPPORT_DL. */
struct bluetooth_nv_item {
    uint8_t address[6];
    uint8_t reserved[58];
};

enum { NV_READ_F = 0, NV_BD_ADDR_I = 1 };

int main(void)
{
    void *library = dlopen("/vendor/lib/libbtnv.so", RTLD_NOW);
    if (!library)
        return 1;

    int (*read_bluetooth_nv)(int, int, struct bluetooth_nv_item *, int) =
        (int (*)(int, int, struct bluetooth_nv_item *, int))dlsym(library, "bt_nv_cmd");
    struct bluetooth_nv_item item = {{0}, {0}};
    int result = read_bluetooth_nv ? read_bluetooth_nv(NV_READ_F, NV_BD_ADDR_I, &item, 0) : -1;
    dlclose(library);
    if (result != 0)
        return 1;

    unsigned nonzero = 0;
    unsigned not_ff = 0;
    for (unsigned i = 0; i < sizeof(item.address); ++i) {
        nonzero |= item.address[i];
        not_ff |= item.address[i] ^ 0xff;
    }
    if (!nonzero || !not_ff)
        return 1;

    /* NV stores HCI byte order; the framework property uses display order. */
    char address[18];
    snprintf(address, sizeof(address), "%02x:%02x:%02x:%02x:%02x:%02x",
             item.address[5], item.address[4], item.address[3],
             item.address[2], item.address[1], item.address[0]);
    return property_set("ro.boot.btmacaddr", address) != 0;
}
