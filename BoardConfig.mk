#
# Copyright (C) 2013-2016 The CyanogenMod Project
#               2017 The LineageOS Project
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

# Board device path
DEVICE_PATH := device/nokia/fame

# OTA assert
TARGET_OTA_ASSERT_DEVICE := fame,fame_cmcc,fame_lta,fame_tmo

# Board device headers
TARGET_SPECIFIC_HEADER_PATH := $(DEVICE_PATH)/include

# Platform
TARGET_BOARD_PLATFORM := msm8960
TARGET_BOOTLOADER_BOARD_NAME := qcom
BOARD_USES_QCOM_HARDWARE := true

# Architecture
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_VARIANT := krait

# Kernel source / bootimg info
TARGET_KERNEL_SOURCE := kernel/nokia/msm8x27
TARGET_KERNEL_CONFIG := lineage_fame_defconfig
BOARD_KERNEL_BASE := 0x80200000
BOARD_KERNEL_PAGESIZE := 4096
BOARD_KERNEL_CMDLINE := panic=3 \
    console=ttyHSL0,115200,n8 \
    androidboot.hardware=qcom \
    user_debug=23 \
    msm_rtb.filter=0x3F \
    ehci-hcd.park=3 \
    androidboot.bootdevice=msm_sdcc.1
ifeq ($(TARGET_BUILD_VARIANT),eng)
# set selinux permissive if it's an eng build
BOARD_KERNEL_CMDLINE += androidboot.selinux=permissive
endif
BOARD_MKBOOTIMG_ARGS := --ramdisk_offset 0x02000000

# Audio configurations
BOARD_USES_ALSA_AUDIO := true
AUDIO_FEATURE_ENABLED_HWDEP_CAL := true
BOARD_USES_FLUENCE_INCALL := true
BOARD_USES_LEGACY_ALSA_AUDIO := true
BOARD_USES_SEPERATED_AUDIO_INPUT := true
BOARD_USES_SEPERATED_VOICE_SPEAKER_MIC := true
QCOM_CSDCLIENT_ENABLED := false
QCOM_USBAUDIO_ENABLED := true

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_QCOM := true
BLUETOOTH_HCI_USE_MCT := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(DEVICE_PATH)/bluetooth

# Camera
USE_DEVICE_SPECIFIC_CAMERA := true

# Device init
TARGET_INIT_VENDOR_LIB := libinit_fame
TARGET_LIBINIT_DEFINES_FILE := $(DEVICE_PATH)/init/init_fame.cpp
TARGET_UNIFIED_DEVICE := true

# Display
TARGET_DISPLAY_USE_RETIRE_FENCE := true
TARGET_USES_C2D_COMPOSITION := true
OVERRIDE_RS_DRIVER := libRSDriver_adreno.so
TARGET_USES_ION := true
MAX_EGL_CACHE_KEY_SIZE := 12*1024
MAX_EGL_CACHE_SIZE := 2048*1024

# FM radio
QCOM_FM_ENABLED := true

# Hardware Class
BOARD_HARDWARE_CLASS := $(DEVICE_PATH)/cmhw

# Healthd
BOARD_CHARGER_ENABLE_SUSPEND := true
BACKLIGHT_PATH := /sys/class/backlight/msmfb_bl0/brightness

# Lights
TARGET_PROVIDES_LIBLIGHT := true

# Partitions informations
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x01400000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x01400000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1258291200
BOARD_USERDATAIMAGE_PARTITION_SIZE := 6149881344
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 2235547136
BOARD_FLASH_BLOCK_SIZE := 131072
TARGET_USERIMAGES_USE_EXT4 := true

# Power HAL
CM_POWERHAL_EXTENSION := qcom
TARGET_POWERHAL_VARIANT := qcom

# Recovery
# TWRP configs only get set if we're building TWRP
ifeq ($(RECOVERY_VARIANT),twrp)
RECOVERY_VARIANT := twrp
TW_THEME := portrait_mdpi
RECOVERY_SDCARD_ON_DATA := true
TW_NO_USB_STORAGE := true
TW_USE_TOOLBOX := true
endif
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/rootdir/fstab.qcom

# RIL
BOARD_RIL_CLASS := ../../../$(DEVICE_PATH)/ril/

# SELinux Policy
include device/qcom/sepolicy/sepolicy.mk
BOARD_SEPOLICY_DIRS += \
    $(DEVICE_PATH)/sepolicy

# Vendor
BOARD_VENDOR := nokia
BOARD_VENDOR_PLATFORM := fame
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

# WiFi
BOARD_HAS_QCOM_WLAN              := true
BOARD_WLAN_DEVICE                := qcwcn
BOARD_HOSTAPD_DRIVER             := NL80211
BOARD_HOSTAPD_PRIVATE_LIB        := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_WPA_SUPPLICANT_DRIVER      := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
WIFI_DRIVER_MODULE_PATH          := "/system/lib/modules/wlan.ko"
WIFI_DRIVER_MODULE_NAME          := "wlan"
WIFI_DRIVER_FW_PATH_STA          := "sta"
WIFI_DRIVER_FW_PATH_AP           := "ap"
WPA_SUPPLICANT_VERSION           := VER_0_8_X

# vendor config inclusions are always last
-include vendor/nokia/fame/BoardConfigVendor.mk
