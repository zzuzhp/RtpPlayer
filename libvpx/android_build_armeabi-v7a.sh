ANDROID_NDK=$USER_HOME/android-env/android-ndk-r10d
PREFIX=`pwd`/libs/armeabi-v7a

./configure --prefix=$PREFIX \
            --target=armv7-android-gcc \
            --disable-runtime-cpu-detect \
            --disable-examples \
            --sdk-path=$ANDROID_NDK \
            --extra-cflags="-mfloat-abi=softfp -mfpu=neon"

make clean
make install

cp lib/armeabi-v7a/lib/libvpx.a $PREFIX
rm -rf lib
