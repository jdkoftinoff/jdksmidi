#
# Makefile to make a (static or shared) library for android system
#

LOCAL_PATH := $(call my-dir)
JDKSMIDI_PATH := $(LOCAL_PATH)/../../

include $(CLEAR_VARS)

LOCAL_PATH := $(JDKSMIDI_PATH)
LOCAL_MODULE:= jdksmidi
LOCAL_MODULE_FILENAME:= libjdksmidi
LOCAL_CFLAGS := -I$(JDKSMIDI_PATH)/include
LOCAL_EXPORT_C_INCLUDES := $(JDKSMIDI_PATH)/include

MY_PREFIX := $(JDKSMIDI_PATH)
MY_SOURCES := $(wildcard $(MY_PREFIX)/src/*.cpp)
LOCAL_SRC_FILES := $(MY_SOURCES:$(MY_PREFIX)%=%)

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)
