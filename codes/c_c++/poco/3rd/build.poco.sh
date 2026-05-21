#!/bin/bash

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
DEBUG_DIR=$ROOT_DIR/.build/linux/x64-Debug
RELEASE_DIR=$ROOT_DIR/.build/linux/x64-Release
BUILD_PARALLEL=$(nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)

if [ "$1" = "debug" ]; then
    mkdir -p "$DEBUG_DIR"
    cd "$DEBUG_DIR" || exit 1

    cmake -C "$ROOT_DIR/poco.cmake" -DCMAKE_BUILD_TYPE=Debug ../../../
    if [ $? -ne 0 ]; then exit 1; fi

    cmake --build . --config Debug --parallel "$BUILD_PARALLEL"
    if [ $? -ne 0 ]; then exit 1; fi

    cd "$ROOT_DIR" || exit 1
elif [ "$1" = "release" ]; then
    mkdir -p "$RELEASE_DIR"
    cd "$RELEASE_DIR" || exit 1

    cmake -C "$ROOT_DIR/poco.cmake" -DCMAKE_BUILD_TYPE=Release ../../../
    if [ $? -ne 0 ]; then exit 1; fi

    cmake --build . --config Release --parallel "$BUILD_PARALLEL"
    if [ $? -ne 0 ]; then exit 1; fi

    cd "$ROOT_DIR" || exit 1
elif [ "$1" = "all" ]; then
    mkdir -p "$DEBUG_DIR"
    cd "$DEBUG_DIR" || exit 1

    cmake -C "$ROOT_DIR/poco.cmake" -DCMAKE_BUILD_TYPE=Debug ../../../
    if [ $? -ne 0 ]; then exit 1; fi

    cmake --build . --config Debug --parallel "$BUILD_PARALLEL"
    if [ $? -ne 0 ]; then exit 1; fi
    echo ""
    echo ""
    echo ""

    cd "$ROOT_DIR" || exit 1

    mkdir -p "$RELEASE_DIR"
    cd "$RELEASE_DIR" || exit 1

    cmake -C "$ROOT_DIR/poco.cmake" -DCMAKE_BUILD_TYPE=Release ../../../
    if [ $? -ne 0 ]; then exit 1; fi

    cmake --build . --config Release --parallel "$BUILD_PARALLEL"
    if [ $? -ne 0 ]; then exit 1; fi

    cd "$ROOT_DIR" || exit 1
else
    echo "Usage: ./build.poco.sh [debug | release | all]"
fi
