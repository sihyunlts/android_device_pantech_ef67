LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libef67_fingerprint
LOCAL_SRC_FILES := Sensor.cpp Engine.cpp
LOCAL_CPPFLAGS := -std=c++11 -Wall -Wextra -Werror
LOCAL_CXX_STL := libc++
LOCAL_SHARED_LIBRARIES := libdl
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
LOCAL_MODULE_TAGS := optional
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := fingerprint.apq8084
LOCAL_SRC_FILES := Fingerprint.cpp
LOCAL_STATIC_LIBRARIES := libef67_fingerprint
LOCAL_SHARED_LIBRARIES := libcrypto libdl liblog
LOCAL_CPPFLAGS := -std=c++11 -Wall -Wextra -Werror
LOCAL_CXX_STL := libc++
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
