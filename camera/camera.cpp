#define LOG_TAG "EF67Camera"

#include <log/log.h>
#include <hardware/camera.h>
#include <hardware/camera_common.h>
#include <dlfcn.h>
#include <errno.h>
#include <mutex>

namespace {
struct CameraState {
    camera_device_t *device = nullptr;
    camera_device_ops_t *stock_ops = nullptr;
    camera_device_ops_t ops = {};
    int (*stock_close)(hw_device_t *) = nullptr;
    camera_request_memory request_memory = nullptr;
    void *callback_user = nullptr;
};

camera_module_t *stock_module;
CameraState cameras[2];
std::mutex cameras_mutex;
CameraState *find_camera(camera_device_t *device) {
    for (auto &camera : cameras) {
        if (camera.device == device) return &camera;
    }
    return nullptr;
}

template <unsigned int Id>
camera_memory_t *request_memory(int fd, size_t size, unsigned int count, void *) {
    camera_request_memory callback;
    void *user;
    {
        std::lock_guard<std::mutex> lock(cameras_mutex);
        callback = cameras[Id].request_memory;
        user = cameras[Id].callback_user;
    }
    // S0226114 passes QCameraStreamMemory here instead of the set_callbacks cookie.
    return callback ? callback(fd, size, count, user) : nullptr;
}

void set_callbacks(camera_device_t *device, camera_notify_callback notify,
        camera_data_callback data, camera_data_timestamp_callback timestamp,
        camera_request_memory memory, void *user) {
    camera_device_ops_t *ops;
    camera_request_memory memory_callback;
    {
        std::lock_guard<std::mutex> lock(cameras_mutex);
        CameraState *camera = find_camera(device);
        if (!camera) return;
        camera->request_memory = memory;
        camera->callback_user = user;
        ops = camera->stock_ops;
        memory_callback = camera == &cameras[0] ? request_memory<0> : request_memory<1>;
    }
    ops->set_callbacks(device, notify, data, timestamp, memory_callback, user);
}

int close_camera(hw_device_t *device) {
    int (*close)(hw_device_t *);
    {
        std::lock_guard<std::mutex> lock(cameras_mutex);
        CameraState *camera = find_camera(reinterpret_cast<camera_device_t *>(device));
        if (!camera) return -EINVAL;
        close = camera->stock_close;
        camera->device->ops = camera->stock_ops;
        device->close = close;
    }
    int result = close(device);
    {
        std::lock_guard<std::mutex> lock(cameras_mutex);
        CameraState *camera = find_camera(reinterpret_cast<camera_device_t *>(device));
        if (camera) *camera = CameraState{};
    }
    return result;
}

int open_camera(const hw_module_t *, const char *name, hw_device_t **device) {
    if (!stock_module) return -ENODEV;
    if (!name || !device || (name[0] != '0' && name[0] != '1') || name[1] != '\0') {
        return -EINVAL;
    }
    std::lock_guard<std::mutex> lock(cameras_mutex);
    CameraState &camera = cameras[name[0] - '0'];
    if (camera.device) return -EBUSY;
    int result = stock_module->common.methods->open(&stock_module->common, name, device);
    if (result != 0) return result;
    camera.device = reinterpret_cast<camera_device_t *>(*device);
    camera.stock_ops = camera.device->ops;
    camera.ops = *camera.stock_ops;
    camera.stock_close = (*device)->close;
    camera.ops.set_callbacks = set_callbacks;
    camera.device->ops = &camera.ops;
    (*device)->close = close_camera;
    return 0;
}

int unavailable_cameras() { return 0; }
int unavailable_camera_info(int, camera_info *) { return -ENODEV; }
hw_module_methods_t methods = { open_camera };
} // namespace

extern "C" {
camera_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = CAMERA_MODULE_API_VERSION_2_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = CAMERA_HARDWARE_MODULE_ID,
        .name = "EF67 Camera1 callback adapter",
        .author = "EF67",
        .methods = &methods,
        .dso = nullptr,
        .reserved = {},
    },
    .get_number_of_cameras = unavailable_cameras,
    .get_camera_info = unavailable_camera_info,
    .set_callbacks = nullptr,
    .get_vendor_tag_ops = nullptr,
    .open_legacy = nullptr,
    .set_torch_mode = nullptr,
    .init = nullptr,
    .reserved = {},
};
}

__attribute__((constructor)) static void load_stock_camera() {
    void *handle = dlopen("/system/lib/hw/camera.apq8084.so", RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        ALOGE("Cannot load stock camera: %s", dlerror());
        return;
    }
    stock_module = static_cast<camera_module_t *>(dlsym(handle, HAL_MODULE_INFO_SYM_AS_STR));
    if (!stock_module) {
        ALOGE("Cannot find stock camera module: %s", dlerror());
        dlclose(handle);
        return;
    }
    // Both the S0226114 symbol and the 32-bit Oreo module have this size.
    static_assert(sizeof(camera_module_t) == 176, "EF67 camera module ABI");
    HAL_MODULE_INFO_SYM = *stock_module;
    HAL_MODULE_INFO_SYM.common.methods = &methods;
}
