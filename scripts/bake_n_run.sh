#!/bin/bash
set -e

if [ -z "$FB_CONFIGURE" ]
then
    echo "Skipping configure..."
else
    echo "Configuring..."
    cmake -S . -B build -G "Visual Studio 17 2022" -T host=x64 -A x64
fi

echo "Building..."
cmake --build ./build --config Debug

if [ -z "$FB_BAKE" ]
then
    echo "Skipping bake..."
else
    echo "Baking..."
    pushd build/src/baker/Debug
    ./fb_baker.exe
    popd
fi

echo "Running..."
pushd build/src/buffet/Debug
./fb_buffet.exe
popd
