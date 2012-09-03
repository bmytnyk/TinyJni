
GPA_BUILD_FLAGS = -DDEBUG -g -Wno-multichar -DTJ_ANDROID -DANDROID_API_LEVEL=9
GPA_ES_VERSION = GLES11
GPA_BUILD_OPTIMIZATION = debug
GPA_API_LEVEL = android-9

LOCAL_PATH 	:= ../Sources
MY_PATH = $(LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_CPPFLAGS  := $(GPA_BUILD_FLAGS)

LOCAL_MODULE    := TinyJni
LOCAL_PATH 	:= $(MY_PATH)
LOCAL_C_INCLUDES = ../Include

LOCAL_SRC_FILES := TJClass.cpp \
				   TJString.cpp \
				   TJCommon.cpp \
				   TJException.cpp \
				   TJJavaException.cpp \
				   TJObjectRef.cpp \
				   TJPrimitiveArray.cpp \
				   TJTypeTraits.cpp \
				   TJValue.cpp
				 

include $(BUILD_SHARED_LIBRARY)
