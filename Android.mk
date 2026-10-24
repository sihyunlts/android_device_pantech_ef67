LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),ef67)
include $(CLEAR_VARS)
LOCAL_MODULE := libtwrp_ef67
LOCAL_SRC_FILES := recovery/recovery.cpp
LOCAL_C_INCLUDES := bootable/recovery
LOCAL_CPPFLAGS := -Wall -Wextra -Werror
include $(BUILD_STATIC_LIBRARY)
endif
