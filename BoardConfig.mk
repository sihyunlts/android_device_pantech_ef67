DEVICE_PATH := device/pantech/ef67

TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := krait
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_USES_64_BIT_BINDER := true
TARGET_BOARD_PLATFORM := apq8084
TARGET_BOOTLOADER_BOARD_NAME := apq8084
TARGET_NO_BOOTLOADER := true

# Kernel and QCDT must come from the same LineageOS 16.0 build.
TARGET_PREBUILT_KERNEL := $(DEVICE_PATH)/prebuilt/zImage
BOARD_KERNEL_BASE := 0x00000000
BOARD_KERNEL_PAGESIZE := 4096
BOARD_KERNEL_CMDLINE := console=NULL,115200,n8 androidboot.console=ttyHSL0 androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x1b7 dwc3_msm.cpu_to_affin=1 vmalloc=280M loglevel=0
BOARD_MKBOOTIMG_ARGS := --kernel_offset 0x00008000 --ramdisk_offset 0x01000000 --second_offset 0x00f00000 --tags_offset 0x00000100 --header_version 0
BOARD_KERNEL_SEPARATED_DT := true
BOARD_KERNEL_PREBUILT_DT := true
BOARD_CUSTOM_BUILD_MAKEFILE := $(DEVICE_PATH)/recovery-image.mk
BOARD_BOOTIMAGE_PARTITION_SIZE := 33554432
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 33554432
BOARD_FLASH_BLOCK_SIZE := 524288

TARGET_USERIMAGES_USE_EXT4 := true
TARGET_USERIMAGES_USE_F2FS := true
# The existing recovery formatter must not enable features absent in this 3.10 kernel.
TW_F2FS_LEGACY_FEATURES := true
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/recovery.fstab
RECOVERY_VARIANT := twrp
TARGET_RECOVERY_TWRP_LIB := libtwrp_ef67
RECOVERY_SDCARD_ON_DATA := true

TARGET_SCREEN_WIDTH := 1440
TARGET_SCREEN_HEIGHT := 2560
TARGET_RECOVERY_PIXEL_FORMAT := RGBA_8888
TW_THEME := portrait_hdpi
TW_BRIGHTNESS_PATH := /sys/class/leds/lcd-backlight/brightness
TW_MAX_BRIGHTNESS := 255
TW_DEFAULT_BRIGHTNESS := 128
# The sensor exposes mouse capabilities even when navigation is stopped.
TW_INPUT_BLACKLIST := fpcMouse
TW_NO_USB_STORAGE := true
TW_EXCLUDE_DEFAULT_USB_INIT := true

BOARD_SEPOLICY_DIRS += $(DEVICE_PATH)/sepolicy

# Preserve the formatter before the device wrapper is packaged.
BOARD_RECOVERY_IMAGE_PREPARE = cp -f $(call intermediates-dir-for,EXECUTABLES,mkfs.f2fs)/mkfs.f2fs $(TARGET_RECOVERY_ROOT_OUT)/sbin/mkfs.f2fs.real
