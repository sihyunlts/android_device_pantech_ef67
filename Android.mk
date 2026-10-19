LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),ef67)
include $(CLEAR_VARS)
LOCAL_MODULE := libinit_ef67
LOCAL_SRC_FILES := init/init_ef67.cpp
LOCAL_C_INCLUDES := system/core/init
LOCAL_MODULE_TAGS := optional
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ef67-wifi-init
LOCAL_SRC_FILES := wifi/wifi_init.c
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := libef67_audio_compat
LOCAL_SRC_FILES := audio/audio_compat.c
LOCAL_C_INCLUDES := external/tinyalsa/include
LOCAL_SHARED_LIBRARIES := libtinyalsa
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libef67_ril
LOCAL_SRC_FILES := ril/ril.cpp
LOCAL_C_INCLUDES := hardware/ril/include
LOCAL_CFLAGS := -DRIL_SHLIB
LOCAL_CPPFLAGS := -std=c++11
LOCAL_CXX_STL := libc++
LOCAL_SHARED_LIBRARIES := libdl liblog
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libef67_camera_compat
LOCAL_SRC_FILES := camera/camera_parameters_compat.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libef67_vibrator
LOCAL_SRC_FILES := vibrator/vibrator.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := lights.apq8084
LOCAL_SRC_FILES := lights/lights.c
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := power.apq8084
LOCAL_SRC_FILES := power/power.c
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := dtbToolEF67
LOCAL_SRC_FILES := dtbtool/dtbtool.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_HOST_EXECUTABLE)
include $(CLEAR_VARS)
LOCAL_MODULE := libpn547_fw_platform
LOCAL_SRC_FILES := nfc/firmware_compat.c
LOCAL_SHARED_LIBRARIES := libpn547_fw_platform_stock
LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/firmware
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libpn547_fw_pku
LOCAL_SRC_FILES := nfc/firmware_compat.c
LOCAL_SHARED_LIBRARIES := libpn547_pku_stock
LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/firmware
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

EF67_INSTALLED_KERNEL_MODULES := \
    $(TARGET_OUT)/lib/modules/mhi.ko \
    $(TARGET_OUT)/lib/modules/qca_cld/qca_cld_wlan.ko

# Keep system image creation dependent on the kernel's module installation.
$(EF67_INSTALLED_KERNEL_MODULES): WLAN_MODULES
ALL_DEFAULT_INSTALLED_MODULES += $(EF67_INSTALLED_KERNEL_MODULES)

include $(LOCAL_PATH)/fingerprint/Android.mk
include device/pantech/ef67/parts/Android.mk
include device/pantech/ef67/bluetooth/Android.mk
endif
