PRODUCT_CHARACTERISTICS := phone
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xxxhdpi
$(call inherit-product, frameworks/native/build/phone-xxhdpi-2048-dalvik-heap.mk)
$(call inherit-product, frameworks/native/build/phone-xxhdpi-2048-hwui-memory.mk)
DEVICE_PACKAGE_OVERLAYS += device/pantech/ef67/overlay

PRODUCT_PACKAGES += mkfs.f2fs fsck.f2fs

PRODUCT_COPY_FILES += \
    device/pantech/ef67/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
    device/pantech/ef67/keylayout/atmel_mxt_641t.kl:system/usr/keylayout/atmel_mxt_641t.kl \
    device/pantech/ef67/media/media_codecs.xml:system/etc/media_codecs.xml \
    device/pantech/ef67/media/media_profiles.xml:system/etc/media_profiles.xml \
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
    lights.apq8084 \
    liboverlay \
    libqdutils \
    libqservice

PRODUCT_PACKAGES += \
    libc2dcolorconvert \
    libstagefrighthw \
    libOmxCore \
    libmm-omxcore \
    libOmxVenc \
    libOmxVdec

PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp

PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=196608 \
    ro.sf.lcd_density=560 \
    persist.hwc.mdpcomp.enable=true

PRODUCT_PACKAGES += ef67-wifi-init

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    device/pantech/ef67/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
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

PRODUCT_PROPERTY_OVERRIDES += \
    ro.qc.sdk.audio.fluencetype=fluence \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.voicerec=false \
    persist.audio.fluence.speaker=true

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml

PRODUCT_PROPERTY_OVERRIDES += \
    qcom.bluetooth.soc=rome \
    ro.bluetooth.hfp.ver=1.6

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml

PRODUCT_PROPERTY_OVERRIDES += \
    ro.telephony.ril_class=QualcommSharedRIL \
    rild.libpath=/system/vendor/lib/libril-qc-qmi-1.so \
    ro.telephony.default_network=9

PRODUCT_PROPERTY_OVERRIDES += \
    ro.use_data_netmgrd=true \
    persist.rmnet.data.enable=true \
    persist.data.wda.enable=true \
    persist.data.netmgrd.qos.enable=false \
    persist.data_netmgrd_mtu=1440 \
    persist.data.df.ul_mode=5 \
    persist.data.df.dl_mode=5 \
    persist.data.df.agg.dl_pkt=10 \
    persist.data.df.agg.dl_size=4096 \
    persist.data.df.mux_count=8 \
    persist.data.df.iwlan_mux=9

PRODUCT_PACKAGES += libef67_camera_compat

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml

$(call inherit-product, vendor/pantech/ef67/ef67-vendor.mk)

PRODUCT_PACKAGES += VegaSettings libvega_settings

PRODUCT_PACKAGES += \
    NfcNci \
    nfc_nci.apq8084 \
    libpn547_fw_platform \
    libpn547_fw_pku

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:system/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:system/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml
