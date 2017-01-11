#!/bin/bash
export TMPDIR=`pwd`/tmp

NDK=/Users/$USER/android-env/android-ndk-r10d
SYSROOT=$NDK/platforms/android-19/arch-arm/
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64

# arm v7a
ABI=armeabi-v7a
CPU_FLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=neon"

CPU=arm
PREFIX=`pwd`/libs/$ABI
ADDI_CFLAGS="$CPU_FLAGS"

ENABLE_AVC_DEC="--enable-decoder=h264 \
                --enable-parser=h264 \
                --enable-bsf=h264_mp4toannexb"

function build_one
{
    mkdir tmp
    rm -rf libs
    mkdir -p libs/$ABI

    ./configure  --prefix=$PREFIX \
                 --disable-shared \
                 --enable-static \
                 --disable-everything \
                 --enable-small \
                 --enable-pthreads \
                 --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
                 --target-os=linux \
                 --arch=arm \
                 --enable-cross-compile \
                 --sysroot=$SYSROOT \
                 --extra-cflags="-Os -fpic $ADDI_CFLAGS" \
                 --extra-ldflags="$ADDI_LDFLAGS" \
                 $ENABLE_AVC_DEC \
                 $ADDITIONAL_CONFIGURE_FLAG

    make clean
    make -j8
    make install

    rm -rf tmp
    rm -rf libs/$ABI/bin libs/$ABI/share
}

build_one
