ifneq ($(filter msm8960 msm8610 msm8916 msm8909,$(TARGET_BOARD_PLATFORM)),)
# Disable temporarily for compilling error
ifneq ($(BUILD_TINY_ANDROID),true)
LOCAL_PATH := $(call my-dir)

# HAL module implemenation stored in
include $(CLEAR_VARS)

ifneq ($(filter msm8610,$(TARGET_BOARD_PLATFORM)),)
  LOCAL_MODULE := sensors.$(TARGET_BOARD_PLATFORM)
  LOCAL_CFLAGS := -DTARGET_8610
else
  ifneq ($(filter msm8916 msm8909,$(TARGET_BOARD_PLATFORM)),)
    LOCAL_MODULE := sensors.$(TARGET_BOARD_PLATFORM)
  else
    LOCAL_MODULE := sensors.msm8930
  endif
endif

ifdef TARGET_2ND_ARCH
LOCAL_MODULE_RELATIVE_PATH := hw
else
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
endif

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -DLOG_TAG=\"Sensors\"
ifeq ($(call is-board-platform,msm8960),true)
  LOCAL_CFLAGS += -DTARGET_8930
endif

LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES :=	\
		sensors.cpp 			\
		SensorBase.cpp			\
		LightSensor.cpp			\
		ProximitySensor.cpp		\
		CompassSensor.cpp		\
		Accelerometer.cpp				\
		Gyroscope.cpp				\
		InputEventReader.cpp \
		CalibrationManager.cpp \
		NativeSensorManager.cpp \
		VirtualSensor.cpp	\
		sensors_XML.cpp

LOCAL_C_INCLUDES += external/libxml2/include	\
		    external/icu/icu4c/source/common

LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libxml2 libutils

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libcalmodule_common
LOCAL_SRC_FILES := \
		   algo/common/common_wrapper.c \
		   algo/common/compass/AKFS_AOC.c \
		   algo/common/compass/AKFS_Device.c \
		   algo/common/compass/AKFS_Direction.c \
		   algo/common/compass/AKFS_VNorm.c

LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional

ifdef TARGET_2ND_ARCH
LOCAL_MODULE_PATH_32 := $(TARGET_OUT_VENDOR)/lib
LOCAL_MODULE_PATH_64 := $(TARGET_OUT_VENDOR)/lib64
else
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
endif

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := calmodule.cfg
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)
LOCAL_SRC_FILES := calmodule.cfg

include $(BUILD_PREBUILT)

endif #BUILD_TINY_ANDROID
endif #TARGET_BOARD_PLATFORM
