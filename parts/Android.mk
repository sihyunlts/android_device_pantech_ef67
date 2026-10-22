LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libvega_settings
LOCAL_SRC_FILES := jni/touch_mode.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

ef67_settings_icon_dir := $(call intermediates-dir-for,APPS,VegaSettings,,COMMON)/settings-icons
ef67_settings_icon_sources := $(foreach icon,ic_launcher.xml ic_foreground.png ic_background.png, \
    $(wildcard packages/apps/Settings/res/mipmap-*/$(icon)))
ef67_settings_icon_files := $(foreach icon,$(ef67_settings_icon_sources), \
    $(call aapt2-compiled-resource-out-file,$(icon),$(ef67_settings_icon_dir)))
include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := VegaSettings
LOCAL_STATIC_JAVA_LIBRARIES := org.lineageos.platform.sdk
LOCAL_USE_AAPT2 := true
LOCAL_STATIC_ANDROID_LIBRARIES := \
    android-support-v7-appcompat \
    android-support-v7-preference \
    android-support-v7-recyclerview \
    android-support-v14-preference
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res \
    vendor/pantech/ef67/proprietary/vega-settings/res
LOCAL_AAPT_FLAGS := --auto-add-overlay $(addprefix -R ,$(ef67_settings_icon_files))
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
LOCAL_PROGUARD_FLAG_FILES := proguard.flags
LOCAL_REQUIRED_MODULES := libvega_settings
LOCAL_MODULE_TAGS := optional
include frameworks/base/packages/SettingsLib/common.mk
include $(BUILD_PACKAGE)

$(foreach icon,$(ef67_settings_icon_sources), \
    $(eval $(call aapt2-compile-one-resource-file-rule,$(icon), \
        $(call aapt2-compiled-resource-out-file,$(icon),$(ef67_settings_icon_dir)))))

$(my_res_package): $(ef67_settings_icon_files)
