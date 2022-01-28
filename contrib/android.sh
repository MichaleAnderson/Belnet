#!/bin/bash
set -e
set +x

default_abis="armeabi-v7a arm64-v8a x86 x86_64"
build_abis=${ABIS:-$default_abis}

test x$NDK = x && echo "NDK env var not set"
test x$NDK = x && exit 1

echo "building abis: $build_abis"

root="$(readlink -f $(dirname $0)/../)"
out=$root/belnet-jni-$(git describe || echo unknown)
mkdir -p $out
mkdir -p $root/build-android
cd $root/build-android

for abi in $build_abis; do
    mkdir -p build-$abi $out/$abi
    cd build-$abi
    cmake \
        -G 'Unix Makefiles' \
        -DANDROID=ON \
        -DANDROID_ABI=$abi \
        -DANDROID_ARM_MODE=arm \
        -DANDROID_PLATFORM=android-23 \
        -DANDROID_STL=c++_static \
        -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
        -DBUILD_STATIC_DEPS=ON \
        -DBUILD_PACKAGE=ON \
        -DBUILD_SHARED_LIBS=OFF \
        -DBUILD_TESTING=OFF \
        -DBUILD_LIBBELNET=OFF \
        -DWITH_TESTS=OFF \
        -DNATIVE_BUILD=OFF \
        -DSTATIC_LINK=ON \
        -DWITH_SYSTEMD=OFF \
        -DFORCE_BMQ_SUBMODULE=ON \
        -DSUBMODULE_CHECK=OFF \
        -DWITH_LTO=OFF \
        -DCMAKE_BUILD_TYPE=Release \
        $@ $root
    make belnet-android -j${JOBS:-$(nproc)}
    cp jni/libbelnet-android.so $out/$abi/libbelnet-android.so
    cd -
done


echo
echo "build artifacts outputted to $out"
