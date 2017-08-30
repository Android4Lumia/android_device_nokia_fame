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

# Board device path
DEVICE_PATH := device/nokia/fame

-include device/nokia/lumia_8930-common/BoardConfigCommon.mk

# Board device headers
TARGET_SPECIFIC_HEADER_PATH := $(DEVICE_PATH)/include

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(DEVICE_PATH)/bluetooth

# Camera
USE_DEVICE_SPECIFIC_CAMERA := true

# Healthd
BOARD_CHARGER_ENABLE_SUSPEND := true
BACKLIGHT_PATH := /sys/class/backlight/msmfb_bl0/brightness
PRODUCT_DEVICE := fame
PRODUCT_NAME := lineage_fame
PRODUCT_MODEL := fame

# OTA assert
TARGET_OTA_ASSERT_DEVICE := fame_cmcc,fame_lta,fame_row,fame_tmo,glee_cmcc,glee_row

# Partitions informations
BOARD_USERDATAIMAGE_PARTITION_SIZE := 6149881344

# Partitions blocks - I'll leave it here for now, maybe we'll need it - NeoH
BOARD_FLASH_BLOCK_SIZE := 131072

# FM radio
QCOM_FM_ENABLED := true

# Kernel properties
TARGET_KERNEL_CONFIG := lineage_fame_defconfig

# Vendor platform
BOARD_VENDOR_PLATFORM := fame

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

# Display
TARGET_DISPLAY_USE_RETIRE_FENCE := true
TARGET_USES_C2D_COMPOSITION := true

# Enables Adreno RS driver
OVERRIDE_RS_DRIVER := libRSDriver_adreno.so

# Ion
TARGET_USES_ION := true

# Shader cache config options
# Maximum size of the  GLES Shaders that can be cached for reuse.
# Increase the size if shaders of size greater than 12KB are used.
MAX_EGL_CACHE_KEY_SIZE := 12*1024

# Maximum GLES shader cache size for each app to store the compiled shader
# binaries. Decrease the size if RAM or Flash Storage size is a limitation
# of the device.
MAX_EGL_CACHE_SIZE := 2048*1024

# RIL
BOARD_RIL_CLASS := ../../../$(DEVICE_PATH)/ril/

# Board device vendor
-include vendor/nokia/fame/BoardConfigVendor.mk
