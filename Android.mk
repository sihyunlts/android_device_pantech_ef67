LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),ef67)
include $(CLEAR_VARS)
LOCAL_MODULE := libinit_ef67
LOCAL_SRC_FILES := init/init_ef67.c
LOCAL_C_INCLUDES := system/core/init
LOCAL_MODULE_TAGS := optional
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := dtbToolEF67
LOCAL_SRC_FILES := dtbtool/dtbtool.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_HOST_EXECUTABLE)
endif
