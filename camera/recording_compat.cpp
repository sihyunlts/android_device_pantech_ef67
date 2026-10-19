#include <camera/CameraParameters.h>
#include <cutils/native_handle.h>
#include <cstring>

extern "C" native_handle_t *ef67_camera_native_handle_create(int num_fds, int num_ints)
{
    // The KitKat recording handle omits the color-space flags used by CAF.
    const bool recording_handle = num_fds == 1 && num_ints == 2;
    native_handle_t *handle = native_handle_create(num_fds, num_ints + recording_handle);
    if (handle && recording_handle)
        handle->data[3] = 0;
    return handle;
}

extern "C" void ef67_camera_parameters_unflatten(
        android::CameraParameters *parameters, const android::String8 &flattened)
{
    parameters->unflatten(flattened);
    const char *recording = parameters->get(android::CameraParameters::KEY_RECORDING_HINT);
    if (!recording || std::strcmp(recording, "true") != 0)
        return;

    int min_fps = 0, max_fps = 0;
    parameters->getPreviewFpsRange(&min_fps, &max_fps);
    // S0226114 uses a fixed 30 FPS range for normal video recording.
    if (min_fps == 5000 && max_fps == 30330)
        parameters->set(android::CameraParameters::KEY_PREVIEW_FPS_RANGE, "30000,30000");
}
