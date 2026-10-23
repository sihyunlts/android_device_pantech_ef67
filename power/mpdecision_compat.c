#include <stdint.h>

/* S0226114 mpdecision imports the former libcutils atomic load export. */
int32_t android_atomic_acquire_load(const volatile int32_t *address)
{
    return __atomic_load_n(address, __ATOMIC_ACQUIRE);
}
