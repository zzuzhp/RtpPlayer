APP_CFLAGS   := -DNDEBUG -fno-strict-aliasing
APP_CPPFLAGS := -std=c++11 -DASIO_STANDALONE -frtti -fexceptions
APP_OPTIM    := release
APP_LDFLAGS  :=

APP_PLATFORM := android-19
APP_ABI      := armeabi-v7a
APP_STL      := gnustl_static
