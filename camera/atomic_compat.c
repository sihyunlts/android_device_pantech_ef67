#include <stdint.h>

int32_t android_atomic_inc(volatile int32_t *address)
{
    return __atomic_fetch_add(address, 1, __ATOMIC_RELEASE);
}

int32_t android_atomic_dec(volatile int32_t *address)
{
    return __atomic_fetch_sub(address, 1, __ATOMIC_RELEASE);
}
