LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := logJNI
LOCAL_SRC_FILES := logJNI.cpp
LOCAL_CFLAGS    := -Werror
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
