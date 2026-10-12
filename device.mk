PRODUCT_CHARACTERISTICS := phone
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xxxhdpi
$(call inherit-product, frameworks/native/build/phone-xxhdpi-2048-dalvik-heap.mk)
$(call inherit-product, frameworks/native/build/phone-xxhdpi-2048-hwui-memory.mk)
DEVICE_PACKAGE_OVERLAYS += device/pantech/ef67/overlay

PRODUCT_COPY_FILES += \
    device/pantech/ef67/keylayout/atmel_mxt_641t.kl:system/usr/keylayout/atmel_mxt_641t.kl \
    device/pantech/ef67/media/media_codecs.xml:system/etc/media_codecs.xml \
    device/pantech/ef67/rootdir/fstab.qcom:root/fstab.qcom \
    device/pantech/ef67/rootdir/init.qcom.rc:root/init.qcom.rc \
    device/pantech/ef67/rootdir/init.ef67.hardware.rc:root/init.ef67.hardware.rc \
    device/pantech/ef67/rootdir/init.qcom.usb.rc:root/init.qcom.usb.rc \
    device/pantech/ef67/rootdir/ueventd.qcom.rc:root/ueventd.qcom.rc

PRODUCT_PACKAGES += \
    e2fsck \
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

PRODUCT_PACKAGES += ef67-wifi-init

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    device/pantech/ef67/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf \
    device/pantech/ef67/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf

PRODUCT_PROPERTY_OVERRIDES += wifi.interface=wlan0

PRODUCT_PACKAGES += \
    audio_policy.apq8084 \
    audiod \
    libtinyalsa \
    libtinycompress \
    libaudioroute \
    audio.a2dp.default \
    audio.usb.default \
    audio.r_submix.default \
    libef67_audio_compat

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml

PRODUCT_PROPERTY_OVERRIDES += \
    qcom.bluetooth.soc=rome \
    ro.bluetooth.hfp.ver=1.6

PRODUCT_PACKAGES += mhi.ko

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml

PRODUCT_PROPERTY_OVERRIDES += \
    ro.telephony.ril_class=QualcommSharedRIL \
    rild.libpath=/system/vendor/lib/libril-qc-qmi-1.so \
    ro.telephony.default_network=9

PRODUCT_PACKAGES += libef67_camera_compat

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml

$(call inherit-product, vendor/pantech/ef67/ef67-vendor.mk)
