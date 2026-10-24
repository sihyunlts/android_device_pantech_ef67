$(call inherit-product, build/target/product/embedded.mk)
$(call inherit-product, vendor/omni/config/common.mk)
$(call inherit-product, device/pantech/ef67/device.mk)

PRODUCT_NAME := omni_ef67
PRODUCT_DEVICE := ef67
PRODUCT_BRAND := VEGA
PRODUCT_MANUFACTURER := PANTECH
PRODUCT_MODEL := IM-A930S
PRODUCT_RELEASE_NAME := ef67
PRODUCT_SHIPPING_API_LEVEL := 19
