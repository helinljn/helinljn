#!/bin/bash

ROOT_DIR=`pwd`
DEBUG_DIR=$ROOT_DIR/.build/linux/x64-Debug
RELEASE_DIR=$ROOT_DIR/.build/linux/x64-Release

if   [ "$1" = "debug" ]; then
    mkdir -p $DEBUG_DIR
    cd $DEBUG_DIR

    cmake -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build . -j $(nproc)

    cd $ROOT_DIR
elif [ "$1" = "release" ]; then
    mkdir -p $RELEASE_DIR
    cd $RELEASE_DIR

    cmake -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build . -j $(nproc)

    cd $ROOT_DIR
elif [ "$1" = "all" ]; then
    mkdir -p $DEBUG_DIR
    cd $DEBUG_DIR

    cmake -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build . -j $(nproc)
    echo ""
    echo ""
    echo ""

    cd $ROOT_DIR

    mkdir -p $RELEASE_DIR
    cd $RELEASE_DIR

    cmake -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build . -j $(nproc)

    cd $ROOT_DIR
else
    echo "Usage: ./build.linux.sh [debug | release | all]"
fi
