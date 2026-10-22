#include <dlfcn.h>
#include <errno.h>
#include <hardware/sensors.h>
#include <sensor/Sensor.h>

#include <mutex>
#include <new>
#include <vector>

namespace {

// S0226114 uses the KitKat sensor_t layout; later fields occupy its reserved space.
static_assert(sizeof(sensor_t) == 68, "32-bit EF67 sensor descriptor ABI");
static_assert(sizeof(sensors_event_t) == 104, "EF67 sensor event ABI");

sensors_module_t *stock_module;
int stock_load_status = -ENODEV;
std::once_flag stock_load_once;
std::once_flag sensor_list_once;
std::vector<android::Sensor> sensor_metadata;
std::vector<sensor_t> sensor_list;
int sensor_count;

int load_stock_module()
{
    std::call_once(stock_load_once, [] {
        void *library = dlopen("/system/vendor/lib/hw/sensors.apq8084.so", RTLD_NOW | RTLD_LOCAL);
        if (!library)
            return;
        stock_module = static_cast<sensors_module_t *>(dlsym(library, HAL_MODULE_INFO_SYM_AS_STR));
        if (stock_module)
            stock_load_status = 0;
        else
            dlclose(library);
        // Keep the module and its descriptor strings loaded for the HAL lifetime.
    });
    return stock_load_status;
}

int get_sensor_list(sensors_module_t *, const sensor_t **list)
{
    int result = load_stock_module();
    if (result)
        return result;
    std::call_once(sensor_list_once, [] {
        const sensor_t *stock_list;
        sensor_count = stock_module->get_sensors_list(stock_module, &stock_list);
        if (sensor_count <= 0)
            return;
        sensor_metadata.reserve(sensor_count);
        sensor_list.reserve(sensor_count);
        for (int i = 0; i < sensor_count; ++i) {
            sensor_metadata.emplace_back(&stock_list[i], SENSORS_DEVICE_API_VERSION_1_1);
            const android::Sensor &metadata = sensor_metadata.back();
            sensor_t converted = stock_list[i];
            converted.stringType = metadata.getStringType().string();
            converted.requiredPermission = metadata.getRequiredPermission().string();
            converted.maxDelay = metadata.getMaxDelay();
            converted.flags = metadata.getFlags();
            converted.reserved[0] = converted.reserved[1] = nullptr;
            sensor_list.push_back(converted);
        }
    });
    *list = sensor_list.data();
    return sensor_count;
}

struct SensorDevice {
    sensors_poll_device_1_t device;
    sensors_poll_device_1_t *stock;
};

SensorDevice *device_from(sensors_poll_device_t *device)
{
    return reinterpret_cast<SensorDevice *>(device);
}

int open_device(const hw_module_t *module, const char *id, hw_device_t **output)
{
    int result = load_stock_module();
    if (result)
        return result;
    sensors_poll_device_1_t *stock;
    result = stock_module->common.methods->open(&stock_module->common, id,
            reinterpret_cast<hw_device_t **>(&stock));
    if (result)
        return result;
    if (stock->common.version != SENSORS_DEVICE_API_VERSION_1_1 ||
            !stock->activate || !stock->setDelay || !stock->poll || !stock->batch || !stock->flush) {
        stock->common.close(&stock->common);
        return -ENODEV;
    }
    SensorDevice *wrapper = new (std::nothrow) SensorDevice{};
    if (!wrapper) {
        stock->common.close(&stock->common);
        return -ENOMEM;
    }
    wrapper->stock = stock;
    wrapper->device.common.tag = HARDWARE_DEVICE_TAG;
    wrapper->device.common.version = SENSORS_DEVICE_API_VERSION_1_3;
    wrapper->device.common.module = const_cast<hw_module_t *>(module);
    wrapper->device.common.close = [](hw_device_t *device) {
        SensorDevice *wrapper = reinterpret_cast<SensorDevice *>(device);
        int result = wrapper->stock->common.close(&wrapper->stock->common);
        delete wrapper;
        return result;
    };
    wrapper->device.activate = [](sensors_poll_device_t *device, int handle, int enabled) {
        SensorDevice *wrapper = device_from(device);
        return wrapper->stock->activate(&wrapper->stock->v0, handle, enabled);
    };
    wrapper->device.setDelay = [](sensors_poll_device_t *device, int handle, int64_t period) {
        SensorDevice *wrapper = device_from(device);
        return wrapper->stock->setDelay(&wrapper->stock->v0, handle, period);
    };
    wrapper->device.poll = [](sensors_poll_device_t *device, sensors_event_t *events, int count) {
        SensorDevice *wrapper = device_from(device);
        return wrapper->stock->poll(&wrapper->stock->v0, events, count);
    };
    wrapper->device.batch = [](sensors_poll_device_1_t *device, int handle, int flags,
            int64_t period, int64_t latency) {
        SensorDevice *wrapper = reinterpret_cast<SensorDevice *>(device);
        return wrapper->stock->batch(wrapper->stock, handle, flags, period, latency);
    };
    wrapper->device.flush = [](sensors_poll_device_1_t *device, int handle) {
        SensorDevice *wrapper = reinterpret_cast<SensorDevice *>(device);
        return wrapper->stock->flush(wrapper->stock, handle);
    };
    *output = &wrapper->device.common;
    return 0;
}

hw_module_methods_t module_methods = { .open = open_device };

} // namespace

extern "C" {
sensors_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = SENSORS_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SENSORS_HARDWARE_MODULE_ID,
        .name = "EF67 legacy sensor metadata adapter",
        .author = "EF67",
        .methods = &module_methods,
    },
    .get_sensors_list = get_sensor_list,
};
}
