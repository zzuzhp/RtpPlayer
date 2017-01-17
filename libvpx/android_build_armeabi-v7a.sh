ANDROID_NDK=/Users/$(whoami)/android-env/android-ndk-r10d/
PREFIX=`pwd`/libs/armeabi-v7a

./configure --prefix=$PREFIX \
            --target=armv7-android-gcc \
            --disable-runtime-cpu-detect \
            --disable-examples \
            --sdk-path=$ANDROID_NDK \
            --extra-cflags="-mfloat-abi=softfp -mfpu=neon"

make clean
make install

cp $PREFIX/lib/libvpx.a $PREFIX
rm -rf $PREFIX/lib/ $PREFIX/include/
