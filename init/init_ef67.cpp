#include <stdio.h>
#include <string.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "vendor_init.h"

static void set_model_property(const char *name, const char *value)
{
    const prop_info *property = __system_property_find(name);

    /* Properties may already be loaded when the vendor hook runs. */
    if (property)
        __system_property_update((prop_info *)property, value, strlen(value));
    else
        __system_property_add(name, strlen(name), value, strlen(value));
}

void vendor_load_properties(void)
{
    static const struct {
        const char *model;
        const char *device;
    } variants[] = {
        { "IM-A930S", "ef67s" },
        { "IM-A930K", "ef67k" },
        { "IM-A930L", "ef67l" },
    };
    unsigned char phoneinfo[0x2d];
    FILE *file;
    size_t bytes_read;
    unsigned int i;

    file = fopen("/dev/block/platform/msm_sdcc.1/by-name/phoneinfo", "rb");
    if (!file)
        return;
    bytes_read = fread(phoneinfo, 1, sizeof(phoneinfo), file);
    fclose(file);

    if (bytes_read != sizeof(phoneinfo) ||
        memcmp(phoneinfo, "$PDL_PHONE_INFO_MAGIC$", sizeof("$PDL_PHONE_INFO_MAGIC$")) ||
        phoneinfo[0x2c] != '\0')
        return;

    for (i = 0; i < sizeof(variants) / sizeof(variants[0]); i++) {
        if (memcmp(phoneinfo + 0x24, variants[i].model, 8))
            continue;
        set_model_property("ro.product.model", variants[i].model);
        set_model_property("ro.product.device", variants[i].device);
        return;
    }
}
