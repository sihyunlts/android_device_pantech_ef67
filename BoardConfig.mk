TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := krait
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi

TARGET_USES_64_BIT_BINDER := true
TARGET_LD_SHIM_LIBS += /system/bin/mpdecision|libef67_mpdecision_compat.so
DEVICE_MANIFEST_FILE := device/pantech/ef67/manifest.xml

TARGET_BOARD_PLATFORM := apq8084
TARGET_BOOTLOADER_BOARD_NAME := apq8084
TARGET_NO_BOOTLOADER := true
BOARD_USES_QCOM_HARDWARE := true
TARGET_USES_ION := true
USE_OPENGL_RENDERER := true
MAX_EGL_CACHE_KEY_SIZE := 12*1024
MAX_EGL_CACHE_SIZE := 2048*1024
BOARD_USES_ALSA_AUDIO := true
BOARD_USES_QC_TIME_SERVICES := true
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_QCOM := true
BOARD_HAS_QCA_BT_ROME := true
QCOM_BT_USE_BTNV := true
BOARD_RIL_CLASS := ../../../device/pantech/ef67/ril
USE_CAMERA_STUB := false
# The stock KitKat camera uses the legacy Camera1 service and metadata ABI.
TARGET_HAS_LEGACY_CAMERA_HAL1 := true
# The stock JPEG library contains text relocations.
TARGET_NEEDS_PLATFORM_TEXT_RELOCATIONS := true
TARGET_INIT_VENDOR_LIB := libinit_ef67
BOARD_HARDWARE_CLASS := device/pantech/ef67/lineagehw

TARGET_KERNEL_SOURCE := kernel/pantech/apq8084
TARGET_KERNEL_CONFIG := ef67_defconfig
BOARD_KERNEL_IMAGE_NAME := zImage
BOARD_KERNEL_SEPARATED_DT := true
NEED_KERNEL_MODULE_SYSTEM := true

# The custom module target replaces the common installer dependency.
INSTALLED_KERNEL_MODULES: TARGET_KERNEL_BINARIES

.PHONY: WLAN_MODULES
WLAN_MODULES: INSTALLED_KERNEL_MODULES
	mkdir -p $(TARGET_OUT)/lib/modules/qca_cld $(TARGET_OUT)/etc/firmware/wlan/qca_cld
	mv $(TARGET_OUT)/lib/modules/wlan.ko $(TARGET_OUT)/lib/modules/qca_cld/qca_cld_wlan.ko
	ln -sf /system/lib/modules/qca_cld/qca_cld_wlan.ko $(TARGET_OUT)/lib/modules/wlan.ko
	ln -sf /data/misc/wifi/WCNSS_qcom_cfg.ini $(TARGET_OUT)/etc/firmware/wlan/qca_cld/WCNSS_qcom_cfg.ini
	ln -sf /data/misc/wifi/wlan_mac.bin $(TARGET_OUT)/etc/firmware/wlan/qca_cld/wlan_mac.bin
TARGET_KERNEL_MODULES += WLAN_MODULES

# S0226114 boot header.
BOARD_KERNEL_BASE := 0x00000000
BOARD_KERNEL_PAGESIZE := 4096
BOARD_KERNEL_CMDLINE := console=NULL,115200,n8 androidboot.console=ttyHSL0 androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x1b7 dwc3_msm.cpu_to_affin=1 vmalloc=280M loglevel=0
BOARD_MKBOOTIMG_ARGS := --kernel_offset 0x00008000 \
    --ramdisk_offset 0x01000000 \
    --second_offset 0x00f00000 \
    --tags_offset 0x00000100

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 33554432
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 33554432
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2566914048
BOARD_USERDATAIMAGE_PARTITION_SIZE := 26503790080
BOARD_CACHEIMAGE_PARTITION_SIZE := 419430400
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
# eMMC erase size; raw images do not contain NAND spare bytes.
BOARD_FLASH_BLOCK_SIZE := 524288
TARGET_RECOVERY_FSTAB := device/pantech/ef67/rootdir/fstab.qcom
BOARD_CUSTOM_BOOTIMG_MK := device/pantech/ef67/bootimage.mk
TARGET_FS_CONFIG_GEN := device/pantech/ef67/config.fs

include device/qcom/sepolicy/sepolicy.mk

BOARD_SEPOLICY_DIRS += device/pantech/ef67/sepolicy

TARGET_PROVIDES_LIBLIGHT := true
BOARD_NFC_CHIPSET := pn547
BOARD_NFC_HAL_SUFFIX := $(TARGET_BOARD_PLATFORM)

WIFI_DRIVER_FW_PATH_STA := "sta"
WIFI_DRIVER_FW_PATH_AP := "ap"

include vendor/pantech/ef67/BoardConfigVendor.mk
