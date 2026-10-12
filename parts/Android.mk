LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libvega_settings
LOCAL_SRC_FILES := jni/touch_mode.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

ef67_settings_icon_dir := $(call intermediates-dir-for,APPS,VegaSettings,,COMMON)/settings-icons
include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := VegaSettings
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res $(ef67_settings_icon_dir)
LOCAL_AAPT_FLAGS := --auto-add-overlay
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
LOCAL_REQUIRED_MODULES := libvega_settings
LOCAL_MODULE_TAGS := optional
include $(BUILD_PACKAGE)

ef67_settings_icon_sources := $(wildcard packages/apps/Settings/res/mipmap-*/ic_launcher_settings.png)
ef67_settings_icon_files := $(patsubst packages/apps/Settings/res/%,$(ef67_settings_icon_dir)/%,$(ef67_settings_icon_sources))

$(ef67_settings_icon_files): $(ef67_settings_icon_dir)/%: packages/apps/Settings/res/%
	$(copy-file-to-target-with-cp)

$(R_file_stamp) $(LOCAL_BUILT_MODULE): $(ef67_settings_icon_files)
