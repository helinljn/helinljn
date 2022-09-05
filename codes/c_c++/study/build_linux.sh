#!/bin/bash

if   [ "$1" = "debug" ]; then
    mkdir -p build/linux/x64-Debug && cd build/linux/x64-Debug
    cmake -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build . --target all -- -j $(nproc)
elif [ "$1" = "release" ]; then
    mkdir -p build/linux/x64-Release && cd build/linux/x64-Release
    cmake -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build . --target all -- -j $(nproc)
elif [ "$1" = "all" ]; then
    mkdir -p build/linux/x64-Debug && cd build/linux/x64-Debug
    cmake -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build . --target all -- -j $(nproc)

    cd ../../../
    mkdir -p build/linux/x64-Release && cd build/linux/x64-Release
    cmake -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build . --target all -- -j $(nproc)
else
    echo "Usage:./build_linux.sh [debug | release | all]"
    exit
fi
