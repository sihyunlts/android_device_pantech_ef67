TARGET_SCREEN_WIDTH := 1440
TARGET_SCREEN_HEIGHT := 2560

$(call inherit-product, device/pantech/ef67/full_ef67.mk)
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

PRODUCT_NAME := cm_ef67
