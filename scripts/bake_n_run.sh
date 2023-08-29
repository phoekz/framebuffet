#!/bin/bash
set -e

if [ "$FB_VARIANT" == "Debug" ]; then
    echo "Debug build"
elif [ "$FB_VARIANT" == "Release" ]; then
    echo "Release build"
else
    echo "FB_VARIANT must be defined and must be either Debug or Release"
    exit 1
fi

if [ -z "$FB_CONFIGURE" ]; then
    echo "Skipping configure..."
else
    echo "Configuring..."
    cmake -S . -B build -G "Visual Studio 17 2022" -T host=x64 -A x64
fi


if [ -z "$FB_BAKE" ]; then
    echo "Skipping bake..."
else
    echo "Building..."
    cmake --build ./build --config $FB_VARIANT --target fb_baker

    echo "Baking..."
    pushd build/src/baker/$FB_VARIANT
    ./fb_baker.exe
    popd
fi

if [ -z "$FB_CLEAN_ARCHIVE" ]; then
    echo "Skipping archive cleaning..."
else
    echo "Cleaning archive files..."
    fd -I "fb_archive.bin" -X rm
fi

if [ -z "$FB_RUN" ]; then
    echo "Skipping run..."
else
    echo "Building ${FB_RUN}..."
    cmake --build ./build --config $FB_VARIANT --target "fb_${FB_RUN}"

    echo "Running ${FB_RUN}..."
    pushd build/src/${FB_RUN}/$FB_VARIANT
    ./fb_${FB_RUN}.exe
    popd
fi
