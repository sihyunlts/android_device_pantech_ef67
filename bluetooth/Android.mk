LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ef67-hci-filter
LOCAL_SRC_FILES := wcnss_filter.c wcnss_ibs.c
LOCAL_SHARED_LIBRARIES := libcutils liblog
LOCAL_CFLAGS := -Wall -Wextra -DWCNSS_IBS_ENABLED
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := ef67-bluetooth-init
LOCAL_SRC_FILES := bluetooth_init.c
LOCAL_SHARED_LIBRARIES := libcutils libdl
LOCAL_CFLAGS := -Wall -Wextra -Werror
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
