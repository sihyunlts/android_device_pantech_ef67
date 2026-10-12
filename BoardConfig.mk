TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := krait
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true

TARGET_BOARD_PLATFORM := apq8084
TARGET_BOOTLOADER_BOARD_NAME := apq8084
TARGET_NO_BOOTLOADER := true
BOARD_USES_QCOM_HARDWARE := true
TARGET_USES_QCOM_BSP := true
TARGET_QCOM_DISPLAY_VARIANT := caf
USE_OPENGL_RENDERER := true
BOARD_HAS_VIBRATOR_IMPLEMENTATION := ../../device/pantech/ef67/vibrator/vibrator.c
TARGET_INIT_VENDOR_LIB := libinit_ef67

TARGET_KERNEL_SOURCE := kernel/pantech/apq8084
TARGET_KERNEL_CONFIG := ef67_defconfig
BOARD_KERNEL_IMAGE_NAME := zImage

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
BOARD_NAND_PAGE_SIZE := 4096
BOARD_NAND_SPARE_SIZE := 0
TARGET_RECOVERY_FSTAB := device/pantech/ef67/rootdir/fstab.qcom
BOARD_CUSTOM_BOOTIMG_MK := device/pantech/ef67/bootimage.mk

BOARD_SEPOLICY_DIRS += device/pantech/ef67/sepolicy
BOARD_SEPOLICY_UNION += \
    file.te \
    file_contexts \
    genfs_contexts \
    ueventd.te \
    vold.te

include vendor/pantech/ef67/BoardConfigVendor.mk
