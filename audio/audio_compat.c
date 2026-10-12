#include <stdlib.h>
#include <tinyalsa/asoundlib.h>

/* S0226114 audio HAL uses this mixer helper from libhardware_legacy. */
int set_value(const char *name, const char *value)
{
    struct mixer *mixer;
    struct mixer_ctl *control;
    int result = -1;

    if (!name || !value)
        return -1;
    mixer = mixer_open(0);
    if (!mixer)
        return -1;
    control = mixer_get_ctl_by_name(mixer, name);
    if (control) {
        if (value[0] >= '0' && value[0] <= '9')
            result = mixer_ctl_set_value(control, 0, atoi(value));
        else if (mixer_ctl_get_type(control) == MIXER_CTL_TYPE_ENUM)
            result = mixer_ctl_set_enum_by_string(control, value);
    }
    mixer_close(mixer);
    return result ? -1 : 0;
}
