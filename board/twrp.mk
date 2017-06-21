# TWRP configs only get set if we're building TWRP
ifeq ($(RECOVERY_VARIANT),twrp)
RECOVERY_VARIANT := twrp
TW_THEME := portrait_mdpi
#RECOVERY_GRAPHICS_USE_LINELENGTH := true
RECOVERY_SDCARD_ON_DATA := true
TW_NO_USB_STORAGE := true
endif
