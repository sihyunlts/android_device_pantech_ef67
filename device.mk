PRODUCT_CHARACTERISTICS := phone
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xxxhdpi
$(call inherit-product, frameworks/native/build/phone-xxhdpi-2048-dalvik-heap.mk)
$(call inherit-product, frameworks/native/build/phone-xxhdpi-2048-hwui-memory.mk)
DEVICE_PACKAGE_OVERLAYS += device/pantech/ef67/overlay

PRODUCT_COPY_FILES += \
    device/pantech/ef67/rootdir/fstab.qcom:root/fstab.qcom \
    device/pantech/ef67/rootdir/init.qcom.rc:root/init.qcom.rc \
    device/pantech/ef67/rootdir/init.qcom.usb.rc:root/init.qcom.usb.rc \
    device/pantech/ef67/rootdir/ueventd.qcom.rc:root/ueventd.qcom.rc

PRODUCT_PACKAGES += \
    e2fsck \
    lights.apq8084 \
    gralloc.apq8084 \
    hwcomposer.apq8084 \
    copybit.apq8084 \
    memtrack.apq8084 \
    liboverlay \
    libqdutils \
    libqservice

PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp

PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=196608 \
    ro.sf.lcd_density=640 \
    persist.hwc.mdpcomp.enable=true

$(call inherit-product, vendor/pantech/ef67/ef67-vendor.mk)
