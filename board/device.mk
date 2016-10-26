PRODUCT_DEVICE := fame
PRODUCT_NAME := cm_fame
PRODUCT_BRAND := Nokia
PRODUCT_MANUFACTURER := Nokia
PRODUCT_MODEL := fame

# OTA assert
TARGET_OTA_ASSERT_DEVICE := fame,glee

# Device init
TARGET_INIT_VENDOR_LIB := libinit_fame
TARGET_LIBINIT_DEFINES_FILE := $(DEVICE_PATH)/init/init_fame.cpp

# Device unified
TARGET_UNIFIED_DEVICE := true
