ifeq ($(TARGET_BOOTLOADER_BOARD_NAME),z71)


LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libaudio.z71
LOCAL_PRELINK_MODULE := false
LOCAL_BUILT_MODULE_STEM := libaudio

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libmedia \
    libhardware_legacy

ifeq ($TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
LOCAL_LDLIBS += -ldl
endif

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

LOCAL_SRC_FILES += AudioHardware.cpp

LOCAL_CFLAGS += -fno-short-enums

LOCAL_STATIC_LIBRARIES += libaudiointerface
ifeq ($(BOARD_HAVE_BLUETOOTH),true)
  LOCAL_SHARED_LIBRARIES += liba2dp libbinder
endif

include $(BUILD_SHARED_LIBRARY)

PRODUCT_COPY_FILES += $(LOCAL_BUILT_MODULE):system/lib/libaudio.so

endif

