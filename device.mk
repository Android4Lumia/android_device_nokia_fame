#
# Copyright (C) 2013-2016 The CyanogenMod Project
#               2017      The LineageOS Project
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

$(call inherit-product, device/nokia/lumia_8930-common/msm8930.mk)

# Screen density
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := hdpi

# Device specific overlays
DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

# Dalvik heap configurations
$(call inherit-product-if-exists, frameworks/native/build/phone-hdpi-dalvik-heap.mk)

# Audio configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio/audio_effects.conf:system/vendor/etc/audio_effects.conf \
    $(LOCAL_PATH)/audio/audio_platform_info.xml:system/etc/audio_platform_info.xml \
    $(LOCAL_PATH)/audio/mixer_paths.xml:system/etc/mixer_paths.xml

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio/audio_policy_configuration.xml:system/etc/audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:/system/etc/a2dp_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:/system/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:/system/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:/system/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:/system/etc/usb_audio_policy_configuration.xml

# Audio properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.qc.sdk.audio.fluencetype=fluence \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.voicerec=false \
    persist.audio.fluence.audiorec=false \
    persist.audio.fluence.speaker=true

# Multichannel AAC
PRODUCT_PROPERTY_OVERRIDES += \
    media.aac_51_output_enabled=true

# Disbale tunnel mode playback by default
PRODUCT_PROPERTY_OVERRIDES += \
    audio.offload.disable=1

# Bluetooth configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/etc/init.qcom.bt.sh:system/etc/init.qcom.bt.sh

# Bluetooth properties
PRODUCT_PROPERTY_OVERRIDES += \
    qcom.bluetooth.soc=smd \
    ro.qualcomm.bt.hci_transport=smd 

# Camera packages
PRODUCT_PACKAGES += \
    camera.qcom \
    Snap

# Camera SHIM packages
PRODUCT_PACKAGES += \
    libsony

# Google Play Store identifier
PRODUCT_GMS_CLIENTID_BASE := android-sonyericsson

# Display properties
PRODUCT_PROPERTY_OVERRIDES += \
    debug.composition.type=dyn \
    persist.debug.wfd.enable=1 \
    persist.hwc.mdpcomp.enable=true \
    persist.sys.wfd.virtual=0 \
    ro.sf.lcd_density=240

# FM configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/etc/init.qcom.fm.sh:system/etc/init.qcom.fm.sh

# SEC configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/sec_config:system/etc/sec_config

# GPS properties
PRODUCT_PROPERTY_OVERRIDES += \
    persist.gps.qc_nlp_in_use=0 \
    ro.qc.sdk.izat.premium_enabled=1 \
    ro.qc.sdk.izat.service_mask=0x5 \
    ro.gps.agps_provider=1

# Hardware permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml

# Hardware configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/etc/init.qcom.baseband.sh:system/etc/init.qcom.baseband.sh \
    $(LOCAL_PATH)/rootdir/init.class_main.sh:root/init.class_main.sh

# Lights package
PRODUCT_PACKAGES += \
    lights.msm8960 \
    sensors.msm8960

# Init configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/init.qcom.rc:root/init.qcom.rc \
    $(LOCAL_PATH)/rootdir/init.qcom.usb.rc:root/init.qcom.usb.rc \
    $(LOCAL_PATH)/rootdir/init.recovery.qcom.rc:root/init.recovery.qcom.rc \
    $(LOCAL_PATH)/rootdir/init.target.rc:root/init.target.rc

# IO Scheduler
PRODUCT_PROPERTY_OVERRIDES += \
    sys.io.scheduler=bfq

# Keys layouts
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/cyttsp-i2c.kl:system/usr/keylayout/cyttsp-i2c.kl \
    $(LOCAL_PATH)/configs/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
    $(LOCAL_PATH)/configs/keypad_8960.kl:system/usr/keylayout/keypad_8960.kl \
    $(LOCAL_PATH)/configs/msm8930-sitar-snd-card_Button_Jack.kl:system/usr/keylayout/msm8930-sitar-snd-card_Button_Jack.kl \
    $(LOCAL_PATH)/configs/pmic8xxx_pwrkey.kcm:system/usr/keychars/pmic8xxx_pwrkey.kcm \
    $(LOCAL_PATH)/configs/pmic8xxx_pwrkey.kl:system/usr/keylayout/pmic8xxx_pwrkey.kl

# Media configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/media_codecs.xml:system/etc/media_codecs.xml \
    $(LOCAL_PATH)/configs/media_codecs_performance.xml:system/etc/media_codecs_performance.xml \
    $(LOCAL_PATH)/configs/media_profiles.xml:system/etc/media_profiles.xml

# Ramdisk configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/fstab.qcom:root/fstab.qcom \
    $(LOCAL_PATH)/rootdir/fstab.qcom:recovery/root/fstab.qcom \
    $(LOCAL_PATH)/rootdir/ueventd.qcom.rc:root/ueventd.qcom.rc \
    $(LOCAL_PATH)/rootdir/init.rle:root/initlogo.rle

# TWRP (optional)
ifeq ($(RECOVERY_VARIANT),twrp)
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/twrp.fstab:recovery/root/etc/twrp.fstab
endif

# ART properties
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.dex2oat-swap=false

# Audio permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.midi.xml:system/etc/permissions/android.software.midi.xml

# Audio packages
PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.primary.msm8960 \
    audio.r_submix.default \
    audio.usb.default \
    libaudio-resampler

PRODUCT_PACKAGES += \
    libqcomvoiceprocessing

# Audio tools
PRODUCT_PACKAGES += \
    tinymix

# Bluetooth permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml

# Camera permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml

# QCOM-perf properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=libqti-perfd-client.so

# RIL properties
PRODUCT_PROPERTY_OVERRIDES += \
    rild.libargs=-d[SPACE]/dev/smd0 \
    rild.libpath=/system/lib/libril-qc-qmi-1.so \
    ro.telephony.ril_class=NokiaRIL

# Radio properties
PRODUCT_PROPERTY_OVERRIDES += \
    persist.radio.apm_sim_not_pwdn=1 \
    ril.subscription.types=NV,RUIM \
    telephony.lteOnCdmaDevice=0

# Telephony properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.telephony.call_ring.multiple=0

# USB permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml

# Thermal configuration
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/thermald-8930.conf:system/etc/thermald-8930.conf

# WiFi WCNSS configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/wifi/WCNSS_cfg.dat:system/etc/firmware/wlan/prima/WCNSS_cfg.dat \
    $(LOCAL_PATH)/wifi/WCNSS_qcom_cfg.ini:system/etc/firmware/wlan/prima/WCNSS_qcom_cfg.ini \
    $(LOCAL_PATH)/wifi/WCNSS_qcom_wlan_nv.bin:root/persist/WCNSS_qcom_wlan_nv.bin

# WPA supplicant configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf \
    $(LOCAL_PATH)/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf

# Hostapd configurations
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/wifi/hostapd_default.conf:system/etc/hostapd/hostapd_default.conf \
    $(LOCAL_PATH)/wifi/hostapd.accept:system/etc/hostapd/hostapd.accept \
    $(LOCAL_PATH)/wifi/hostapd.deny:system/etc/hostapd/hostapd.deny

# Vendor product configurations
$(call inherit-product, vendor/nokia/fame/fame-vendor.mk)
