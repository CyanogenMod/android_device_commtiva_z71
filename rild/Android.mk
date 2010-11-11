# Copyright 2006 The Android Open Source Project

ifeq ($(FORCE_BUILD_OF_HACKED_QC_RIL),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	rild.c


LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libril

ifeq ($(TARGET_ARCH),arm)
LOCAL_SHARED_LIBRARIES += libdl
endif # arm

LOCAL_CFLAGS := -DRIL_SHLIB

LOCAL_MODULE:= rild

include $(BUILD_EXECUTABLE)

endif # FORCE_BUILD
