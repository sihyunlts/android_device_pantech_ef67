TARGET_SCREEN_WIDTH := 1440
TARGET_SCREEN_HEIGHT := 2560
TARGET_BOOTANIMATION_HALF_RES := true

$(call inherit-product, device/pantech/ef67/full_ef67.mk)
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

PRODUCT_NAME := lineage_ef67
