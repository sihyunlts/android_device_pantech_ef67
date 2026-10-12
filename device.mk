PRODUCT_CHARACTERISTICS := phone
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xxxhdpi
DEVICE_PACKAGE_OVERLAYS += device/pantech/ef67/overlay

PRODUCT_COPY_FILES += \
    device/pantech/ef67/rootdir/fstab.qcom:root/fstab.qcom \
    device/pantech/ef67/rootdir/init.qcom.rc:root/init.qcom.rc \
    device/pantech/ef67/rootdir/init.qcom.usb.rc:root/init.qcom.usb.rc \
    device/pantech/ef67/rootdir/ueventd.qcom.rc:root/ueventd.qcom.rc

PRODUCT_PACKAGES += \
    e2fsck

PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp

$(call inherit-product, vendor/pantech/ef67/ef67-vendor.mk)
