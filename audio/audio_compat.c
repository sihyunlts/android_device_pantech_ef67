#include <stdlib.h>
#include <tinyalsa/asoundlib.h>

#define list_init inline_list_init
#define list_add_tail inline_list_add_tail
#define list_remove inline_list_remove
#include <cutils/list.h>
#undef list_init
#undef list_add_tail
#undef list_remove

/* KitKat exported these helpers; Lollipop provides them only inline. */
void list_init(struct listnode *node)
{
    inline_list_init(node);
}

void list_add_tail(struct listnode *head, struct listnode *item)
{
    inline_list_add_tail(head, item);
}

void list_remove(struct listnode *item)
{
    inline_list_remove(item);
}

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
