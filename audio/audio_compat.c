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

/* S0226114 pcm_config predates tinyalsa's silence_size field. */
struct stock_pcm_config {
    unsigned int channels;
    unsigned int rate;
    unsigned int period_size;
    unsigned int period_count;
    enum pcm_format format;
    unsigned int start_threshold;
    unsigned int stop_threshold;
    unsigned int silence_threshold;
    int avail_min;
};

struct pcm *pcm_open_legacy(unsigned int card, unsigned int device,
                            unsigned int flags, struct stock_pcm_config *stock)
{
    struct pcm *pcm;
    struct pcm_config config;

    if (!stock)
        return pcm_open(card, device, flags, NULL);

    config = (struct pcm_config) {
        .channels = stock->channels,
        .rate = stock->rate,
        .period_size = stock->period_size,
        .period_count = stock->period_count,
        .format = stock->format,
        .start_threshold = stock->start_threshold,
        .stop_threshold = stock->stop_threshold,
        .silence_threshold = stock->silence_threshold,
        .silence_size = 0,
        .avail_min = stock->avail_min,
    };
    pcm = pcm_open(card, device, flags, &config);
    stock->period_size = config.period_size;
    stock->period_count = config.period_count;
    return pcm;
}
