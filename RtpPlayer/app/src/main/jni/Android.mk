LOCAL_PATH      := $(call my-dir)
PRO_PRO_LIB_DIR := $(LOCAL_PATH)/../../../../../pro/bin/
PRO_PRO_INC_DIR := $(LOCAL_PATH)/../../../../../pro/inc/
PRO_DEC_INC_DIR := $(LOCAL_PATH)/../../../../../MediaCodec/jni/
PRO_DEC_LIB_DIR := $(LOCAL_PATH)/../../../../../MediaCodec/libs/$(TARGET_ARCH_ABI)/

#################### prebuild librtp_framework.so ####################
include $(CLEAR_VARS)
LOCAL_MODULE := rtp_framework

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_SRC_FILES := $(PRO_PRO_LIB_DIR)librtp_framework.so
endif

include $(PREBUILT_SHARED_LIBRARY)

#################### prebuild libpro_util.a ####################
include $(CLEAR_VARS)
LOCAL_MODULE := pro_util

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_SRC_FILES := $(PRO_PRO_LIB_DIR)libpro_util.a
endif

include $(PREBUILT_STATIC_LIBRARY)

#################### prebuild libmediacodec.so ####################
include $(CLEAR_VARS)
LOCAL_MODULE := mediacodec

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_SRC_FILES := $(PRO_DEC_LIB_DIR)libmediacodec.so
endif

include $(PREBUILT_SHARED_LIBRARY)

#################### RtpPlayer ####################
include $(CLEAR_VARS)
LOCAL_MODULE := rtpplayer

LOCAL_SRC_FILES  := RtpPlayer_jni.cc                        \
                    Events/EventBus.cc                      \
                    Events/EventQueue.cc                    \
                    Player/RtpSession.cc                    \
                    Player/RtpPlayer.cc                     \
                    Player/AVSync.cc                        \
                    Streams/RtpStream.cc                    \
                    Streams/AudioStream.cc                  \
                    Streams/VideoStream.cc                  \
                    Decoders/AudioDecoder.cc                \
                    Decoders/VideoDecoder.cc                \
                    Renders/AVOutBase.cc                    \
                    Renders/AudioOut.cc                     \
                    Renders/VideoOut.cc                     \
                    Depacketizers/RtpDepacketizer.cc        \
                    Depacketizers/RtpDepacketizerH264.cc    \
                    Depacketizers/RtpDepacketizerVp8.cc

LOCAL_CFLAGS     :=
LOCAL_LDLIBS     := -llog -lGLESv2 -lEGL -lOpenSLES -landroid
LOCAL_C_INCLUDES += $(PRO_PRO_INC_DIR) $(PRO_DEC_INC_DIR)
LOCAL_SHARED_LIBRARIES := librtp_framework libmediacodec
LOCAL_STATIC_LIBRARIES := libpro_util
LOCAL_DISABLE_FORMAT_STRING_CHECKS  := true
LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true

include $(BUILD_SHARED_LIBRARY)
