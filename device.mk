#
# Copyright (C) 2013-2016 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := device/nokia/fame

# Device specific overlays
DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

# Product common configurations
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Dalvik/Art configurations
$(call inherit-product-if-exists, frameworks/native/build/phone-hdpi-512-dalvik-heap.mk)
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.dex2oat-swap=false

# AAPT Config
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := hdpi

# Audio configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio/audio_policy.conf:system/etc/audio_policy.conf \
    $(LOCAL_PATH)/audio/snd_soc_msm_Sitar:system/etc/snd_soc_msm/snd_soc_msm_Sitar

# Audio packages
PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.primary.msm8960 \
    audio.r_submix.default \
    audio.usb.default \
    libaudio-resampler \
    tinymix

# Audio properties
PRODUCT_PROPERTY_OVERRIDES += \
    media.aac_51_output_enabled=true \
    persist.audio.fluence.mode=endfire \
    persist.audio.vr.enable=false \
    persist.audio.handset.mic=digital \
    persist.audio.lowlatency.rec=false \
    qcom.audio.init=complete \
    ro.qc.sdk.audio.ssr=false \
    ro.qc.sdk.audio.fluencetype=fluence

# Bluetooth configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/etc/init.qcom.bt.sh:system/etc/init.qcom.bt.sh

# Bluetooth properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.qualcomm.bt.hci_transport=smd

# Camera packages
PRODUCT_PACKAGES += \
    camera.qcom \
    Snap

# Camera SHIM packages
PRODUCT_PACKAGES += \
    libsony

# Display packages
PRODUCT_PACKAGES += \
    hwcomposer.msm8960 \
    gralloc.msm8960 \
    copybit.msm8960 \
    memtrack.msm8960

# Display properties
PRODUCT_PROPERTY_OVERRIDES += \
    debug.composition.type=dyn \
    persist.debug.wfd.enable=1 \
    persist.hwc.mdpcomp.enable=true \
    persist.sys.wfd.virtual=0 \
    ro.sf.lcd_density=240 \
    ro.opengles.version=196608

# DRM packages
PRODUCT_PACKAGES += \
    com.google.widevine.software.drm

# DRM properties
PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true

# FM packages
PRODUCT_PACKAGES += \
    FMRadio \
    libfmjni

# FM script
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/etc/init.qcom.fm.sh:system/etc/init.qcom.fm.sh

# Google Play Store identifier
PRODUCT_GMS_CLIENTID_BASE := android-google

# GPS config
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/sec_config:system/etc/sec_config
PRODUCT_PROPERTY_OVERRIDES += \
    persist.gps.qc_nlp_in_use=0 \
    ro.qc.sdk.izat.premium_enabled=1 \
    ro.qc.sdk.izat.service_mask=0x5 \
    ro.gps.agps_provider=1

# GPS packages
PRODUCT_PACKAGES += \
    gps.msm8960 \
    gps.conf \
    sap.conf \
    izat.conf

# Hardware configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/etc/init.qcom.baseband.sh:system/etc/init.qcom.baseband.sh \
    $(LOCAL_PATH)/rootdir/init.class_main.sh:root/init.class_main.sh

# Init and fstab
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/init.qcom.rc:root/init.qcom.rc \
    $(LOCAL_PATH)/rootdir/init.qcom.usb.rc:root/init.qcom.usb.rc \
    $(LOCAL_PATH)/rootdir/init.recovery.qcom.rc:root/init.recovery.qcom.rc \
    $(LOCAL_PATH)/rootdir/init.target.rc:root/init.target.rc \
    $(LOCAL_PATH)/rootdir/fstab.qcom:root/fstab.qcom \
    $(LOCAL_PATH)/rootdir/fstab.qcom:recovery/root/fstab.qcom \
    $(LOCAL_PATH)/rootdir/ueventd.qcom.rc:root/ueventd.qcom.rc \
    $(LOCAL_PATH)/rootdir/twrp.fstab:recovery/root/etc/twrp.fstab

# IO Scheduler
PRODUCT_PROPERTY_OVERRIDES += \
    sys.io.scheduler=bfq

# Keylayouts
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/cyttsp-i2c.kl:system/usr/keylayout/cyttsp-i2c.kl \
    $(LOCAL_PATH)/configs/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
    $(LOCAL_PATH)/configs/keypad_8960.kl:system/usr/keylayout/keypad_8960.kl \
    $(LOCAL_PATH)/configs/msm8930-sitar-snd-card_Button_Jack.kl:system/usr/keylayout/msm8930-sitar-snd-card_Button_Jack.kl \
    $(LOCAL_PATH)/configs/pmic8xxx_pwrkey.kcm:system/usr/keychars/pmic8xxx_pwrkey.kcm \
    $(LOCAL_PATH)/configs/pmic8xxx_pwrkey.kl:system/usr/keylayout/pmic8xxx_pwrkey.kl

# Lights package
PRODUCT_PACKAGES += \
    lights.msm8960 \
    sensors.msm8960

# Logo
# There is a weird hack we have to do because certian stuff in root gets copied over top of 
# recovery/root upon ramdisk generation, which makes it so it automatically uses the same
# initlogo for everything (even if we copy the proper initlogo to recovery/root), and 
# that can be found in boot/mkbootimg.mk.
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/logo/initlogo.rle:root/initlogo.rle

# Media profiles
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/media_codecs.xml:system/etc/media_codecs.xml \
    $(LOCAL_PATH)/configs/media_codecs_performance.xml:system/etc/media_codecs_performance.xml \
    $(LOCAL_PATH)/configs/media_profiles.xml:system/etc/media_profiles.xml

# OMX packages
PRODUCT_PACKAGES += \
    libOmxAacEnc \
    libOmxAmrEnc \
    libOmxCore \
    libOmxEvrcEnc \
    libOmxQcelp13Enc \
    libOmxVdec \
    libOmxVenc \
    libc2dcolorconvert \
    libdivxdrmdecrypt \
    libmm-omxcore \
    libstagefrighthw

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/android.software.midi.xml:system/etc/permissions/android.software.midi.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video_le.xml:system/etc/media_codecs_google_video_le.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml

# PowerHAL
PRODUCT_PACKAGES += \
    power.qcom
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=libqti-perfd-client.so

# RIL properties
PRODUCT_PROPERTY_OVERRIDES += \
    rild.libargs=-d[SPACE]/dev/smd0 \
    rild.libpath=/system/lib/libril-qc-qmi-1.so \
    ro.telephony.ril_class=NokiaRIL \
    persist.radio.apm_sim_not_pwdn=1 \
    ril.subscription.types=NV,RUIM \
    telephony.lteOnCdmaDevice=0 \
    ro.telephony.call_ring.multiple=0

# Thermal throttling
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/thermald-8930.conf:system/etc/thermald-8930.conf

# WiFi configs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/wifi/WCNSS_cfg.dat:system/etc/firmware/wlan/prima/WCNSS_cfg.dat \
    $(LOCAL_PATH)/wifi/WCNSS_qcom_cfg.ini:system/etc/firmware/wlan/prima/WCNSS_qcom_cfg.ini \
    $(LOCAL_PATH)/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf \
    $(LOCAL_PATH)/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf \
    $(LOCAL_PATH)/wifi/hostapd_default.conf:system/etc/hostapd/hostapd_default.conf \
    $(LOCAL_PATH)/wifi/hostapd.accept:system/etc/hostapd/hostapd.accept \
    $(LOCAL_PATH)/wifi/hostapd.deny:system/etc/hostapd/hostapd.deny

# WiFi packages
PRODUCT_PACKAGES += \
    dhcpcd.conf \
    hostapd \
    libqsap_sdk \
    libQWiFiSoftApCfg \
    libwpa_client \
    wcnss_service \
    wpa_supplicant \
    wpa_supplicant.conf

# WiFi properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.disableWifiApFirmwareReload=true \
    wifi.interface=wlan0 \
    wlan.driver.ath=0

# Vendor product configurations
$(call inherit-product, vendor/nokia/fame/fame-vendor.mk)
