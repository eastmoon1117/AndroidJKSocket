LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := daemon
LOCAL_SRC_FILES := daemon.cpp protocol.cpp

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \

LOCAL_LDLIBS := -lm -llog

include $(BUILD_EXECUTABLE)
