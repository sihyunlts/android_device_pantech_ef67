#include <ui/Fence.h>

extern "C" android::status_t ef67_fence_wait(android::Fence *fence, unsigned int timeout)
    __asm__("_ZN7android5Fence4waitEj");

extern "C" android::status_t ef67_fence_wait(android::Fence *fence, unsigned int timeout)
{
    return fence->wait(static_cast<int>(timeout));
}
