#include <ui/Fence.h>
#include <cstdint>
#include <unistd.h>

extern "C" android::status_t ef67_fence_wait(android::Fence *fence, unsigned int timeout)
    __asm__("_ZN7android5Fence4waitEj");

extern "C" android::status_t ef67_fence_wait(android::Fence *fence, unsigned int timeout)
{
    return fence->wait(static_cast<int>(timeout));
}

// Pie inlines this destructor, while the stock HAL imports its old symbol.
// Both ABIs store the reference count followed by the owned fence descriptor.
struct LegacyFence {
    int32_t referenceCount;
    int descriptor;
};
static_assert(sizeof(LegacyFence) == sizeof(android::Fence), "EF67 Fence ABI");

extern "C" void ef67_fence_destroy(LegacyFence *fence)
    __asm__("_ZN7android5FenceD1Ev");

extern "C" void ef67_fence_destroy(LegacyFence *fence)
{
    if (fence->descriptor >= 0)
        close(fence->descriptor);
}
