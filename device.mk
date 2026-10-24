PRODUCT_CHARACTERISTICS := phone
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xxxhdpi
# Preserve the existing 3GB/QHD heap profile removed from Pie's presets.
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapgrowthlimit=288m \
    dalvik.vm.heapsize=768m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=2m \
    dalvik.vm.heapmaxfree=8m
DEVICE_PACKAGE_OVERLAYS += device/pantech/ef67/overlay

# Oreo adapters for services reached during initial boot.
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-impl \
    android.hardware.camera.provider@2.4-impl \
    camera.device@1.0-impl \
    android.hardware.gnss@1.0-impl \
    android.hardware.wifi@1.0-service \
    android.hardware.nfc@1.0-impl \
    android.hardware.nfc@1.0-service \
    android.hardware.sensors@1.0-impl \
    android.hardware.sensors@1.0-service \
    android.hardware.audio@2.0-impl \
    android.hardware.audio@2.0-service \
    android.hardware.audio.effect@2.0-impl \
    android.hardware.keymaster@3.0-impl \
    android.hardware.keymaster@3.0-service \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.mapper@2.0-impl \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.composer@2.1-service \
    android.hardware.memtrack@1.0-impl \
    android.hardware.memtrack@1.0-service \
    android.hardware.light@2.0-impl \
    android.hardware.light@2.0-service \
    android.hardware.power@1.0-impl \
    android.hardware.power@1.0-service \
    android.hardware.vibrator@1.0-impl \
    android.hardware.vibrator@1.0-service

PRODUCT_PACKAGES += mkfs.f2fs fsck.f2fs

PRODUCT_PACKAGES += ef67-hci-filter libbt-vendor vibrator.apq8084

PRODUCT_COPY_FILES += \
    device/pantech/ef67/keylayout/fpcSystemNavigation.kl:system/usr/keylayout/fpcSystemNavigation.kl \
    device/pantech/ef67/audio/audio_policy.conf:system/etc/audio_policy.conf \
    device/pantech/ef67/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
    device/pantech/ef67/keylayout/atmel_mxt_641t.kl:system/usr/keylayout/atmel_mxt_641t.kl \
    device/pantech/ef67/media/media_codecs.xml:system/etc/media_codecs.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml \
    device/pantech/ef67/media/media_profiles.xml:system/etc/media_profiles.xml \
    device/pantech/ef67/nfc/libnfc-brcm.conf:system/etc/libnfc-brcm.conf \
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
    power.apq8084 \
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

# Camera1 metadata contains process-local handles. Keep the hardware encoder
# with the legacy camera service; this also moves secure decoders to mediaserver
# and reduces process isolation. Ordinary decoders stay in media.codec.
PRODUCT_PROPERTY_OVERRIDES += \
    media.stagefright.legacyencoder=true \
    media.stagefright.less-secure=true

PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=196610 \
    ro.sf.lcd_density=560 \
    persist.hwc.mdpcomp.enable=true

PRODUCT_PACKAGES += ef67-wifi-init wpa_supplicant.conf

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    device/pantech/ef67/wifi/wpa_supplicant_overlay.conf:system/vendor/etc/wifi/wpa_supplicant_overlay.conf \
    device/pantech/ef67/wifi/p2p_supplicant_overlay.conf:system/vendor/etc/wifi/p2p_supplicant_overlay.conf

PRODUCT_PROPERTY_OVERRIDES += wifi.interface=wlan0

PRODUCT_PACKAGES += \
    audiod \
    libtinyalsa \
    libtinycompress \
    libaudioroute \
    audio.a2dp.default \
    audio.usb.default \
    audio.r_submix.default \
    libef67_audio_compat \
    libef67_qmi_time

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
    ro.telephony.ril_class=EF67RIL \
    persist.radio.oem_socket=false \
    rild.libpath=/system/lib/libef67_ril.so \
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

PRODUCT_PACKAGES += libef67_camera_compat libef67_ril libtime_genoff

PRODUCT_PACKAGES += Snap libboringssl-compat

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.opengles.aep.xml:system/etc/permissions/android.hardware.opengles.aep.xml \
    frameworks/native/data/etc/android.software.midi.xml:system/etc/permissions/android.software.midi.xml

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

PRODUCT_PACKAGES += \
    fingerprint.apq8084 \
    android.hardware.biometrics.fingerprint@2.1-service

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.fingerprint.xml:system/etc/permissions/android.hardware.fingerprint.xml

# Normalize KitKat sensor metadata using the existing Android legacy conversion.
PRODUCT_PACKAGES += sensors.ef67
PRODUCT_PROPERTY_OVERRIDES += ro.hardware.sensors=ef67

# Preserve the framework memory callback cookie for the KitKat camera HAL.
PRODUCT_PACKAGES += camera.ef67
PRODUCT_PROPERTY_OVERRIDES += ro.hardware.camera=ef67

PRODUCT_PACKAGES += libef67_mpdecision_compat
