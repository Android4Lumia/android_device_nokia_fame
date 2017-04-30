
# OTA assert
TARGET_OTA_ASSERT_DEVICE := zeal_row,zeal_cmcc,fame,fame_cmcc,fame_lta,fame_tmo,glee,glee_cmcc

# Device init
TARGET_INIT_VENDOR_LIB := libinit_fame
TARGET_LIBINIT_DEFINES_FILE := $(DEVICE_PATH)/init/init_fame.cpp

# Device unified
TARGET_UNIFIED_DEVICE := true
