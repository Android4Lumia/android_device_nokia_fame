PRODUCT_DEVICE := fame
PRODUCT_NAME := cm_fame
PRODUCT_BRAND := Nokia
PRODUCT_MANUFACTURER := Nokia
PRODUCT_MODEL := fame

# OTA assert
TARGET_OTA_ASSERT_DEVICE := fame,fame_cmcc,fame_lta,fame_tmo,glee,glee_cmcc

# Device init
TARGET_INIT_VENDOR_LIB := libinit_fame
TARGET_LIBINIT_DEFINES_FILE := $(DEVICE_PATH)/init/init_fame.cpp

# Device unified
TARGET_UNIFIED_DEVICE := true

# Platform
TARGET_BOARD_PLATFORM := msm8960

# Architecture
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_VARIANT := krait

# Kernel informations
BOARD_KERNEL_BASE := 0x80200000
BOARD_KERNEL_PAGESIZE := 4096
BOARD_KERNEL_CMDLINE := panic=3 console=ttyHSL0,115200,n8 androidboot.hardware=qcom user_debug=23 msm_rtb.filter=0x3F ehci-hcd.park=3 androidboot.bootdevice=msm_sdcc.1
BOARD_MKBOOTIMG_ARGS := --ramdisk_offset 0x02000000

# Kernel properties
TARGET_KERNEL_SOURCE := kernel/nokia/msm8x27
TARGET_KERNEL_CONFIG := lineage_fame_defconfig

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := qcom

# Vendor platform
BOARD_VENDOR := nokia
BOARD_VENDOR_PLATFORM := fame

# Images
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

# Partitions informations
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x01400000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x01400000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1258291200
BOARD_USERDATAIMAGE_PARTITION_SIZE := 6149881344
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 2235547136

# Partitions types
TARGET_USERIMAGES_USE_EXT4 := true

# Partitions blocks
BOARD_FLASH_BLOCK_SIZE := 131072

# Recovery
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/rootdir/fstab.qcom
RECOVERY_VARIANT := twrp
TW_THEME := portrait_mdpi
#RECOVERY_GRAPHICS_USE_LINELENGTH := true
RECOVERY_SDCARD_ON_DATA := true
TW_NO_USB_STORAGE := true

# RIL
BOARD_RIL_CLASS := ../../../$(DEVICE_PATH)/ril/
