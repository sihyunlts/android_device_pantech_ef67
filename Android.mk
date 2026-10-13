LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),ef67)
include $(CLEAR_VARS)
LOCAL_MODULE := libinit_ef67
LOCAL_SRC_FILES := init/init_ef67.c
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
LOCAL_MODULE := libef67_camera_compat
LOCAL_SRC_FILES := camera/camera_parameters_compat.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := lights.apq8084
LOCAL_SRC_FILES := lights/lights.c
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
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

include $(LOCAL_PATH)/parts/Android.mk
endif
