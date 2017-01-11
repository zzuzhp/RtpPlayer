LOCAL_PATH   := $(call my-dir)
PRO_ROOT_DIR := $(LOCAL_PATH)/../..

######################################## prebuild libx264.a ########################################
include $(CLEAR_VARS)
LOCAL_MODULE := libx264
LOCAL_SRC_FILES := $(PRO_ROOT_DIR)/libx264/libs/$(TARGET_ARCH_ABI)/libx264.a
include $(PREBUILT_STATIC_LIBRARY)

######################################## prebuild libvpx.a #########################################
include $(CLEAR_VARS)
LOCAL_MODULE := libvpx
LOCAL_SRC_FILES := $(PRO_ROOT_DIR)/libvpx/libs/$(TARGET_ARCH_ABI)/libvpx.a
include $(PREBUILT_STATIC_LIBRARY)

######################################## prebuild libopus.a ########################################
include $(CLEAR_VARS)
LOCAL_MODULE := opus
LOCAL_SRC_FILES := $(PRO_ROOT_DIR)/opus/libs/obj/local/armeabi/libopus.a
include $(PREBUILT_STATIC_LIBRARY)

######################################## prebuild libavcodec.a #####################################
include $(CLEAR_VARS)
LOCAL_MODULE := libavcodec
LOCAL_SRC_FILES := $(PRO_ROOT_DIR)/ffmpeg/libs/$(TARGET_ARCH_ABI)/lib/libavcodec.a
include $(PREBUILT_STATIC_LIBRARY)

######################################## prebuild libavutil.a ######################################
include $(CLEAR_VARS)
LOCAL_MODULE := libavutil
LOCAL_SRC_FILES := $(PRO_ROOT_DIR)/ffmpeg/libs/$(TARGET_ARCH_ABI)/lib/libavutil.a
include $(PREBUILT_STATIC_LIBRARY)

######################################## mediacodec ################################################
include $(CLEAR_VARS)
LOCAL_MODULE := mediacodec

LOCAL_SRC_FILES  := videoenc.c \
                    videodec.c \
                    vp8enc.c \
                    vp8dec.c \
                    avcenc.c \
                    avcdec.c \
                    audioenc.c \
                    audiodec.c \
                    opusenc.c \
                    opusdec.c \
                    ivfwriter.c

LOCAL_CFLAGS     := -DMEDIACODEC_EXPORTS
LOCAL_LDLIBS     := -llog
LOCAL_C_INCLUDES += $(PRO_ROOT_DIR)/libx264/ \
                    $(PRO_ROOT_DIR)/libvpx/ \
                    $(PRO_ROOT_DIR)/ffmpeg/libs/$(TARGET_ARCH_ABI)/include/ \
                    $(PRO_ROOT_DIR)/opus/include/ \
                    $(PRO_ROOT_DIR)/opus/celt/

LOCAL_SHARED_LIBRARIES :=
LOCAL_STATIC_LIBRARIES := libvpx libopus libx264 libavcodec libavutil
LOCAL_DISABLE_FORMAT_STRING_CHECKS  := true
LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true

include $(BUILD_SHARED_LIBRARY)
