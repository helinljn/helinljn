#!/bin/bash

ROOT_DIR=`pwd`
DEBUG_DIR=$ROOT_DIR/poco/.build/linux/x64-Debug
RELEASE_DIR=$ROOT_DIR/poco/.build/linux/x64-Release

if   [ "$1" = "debug" ]; then
    mkdir -p $DEBUG_DIR
    cd $DEBUG_DIR

    cmake -C $ROOT_DIR/poco.cmake -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build . --config Debug -j $(nproc)

    cd $ROOT_DIR
elif [ "$1" = "release" ]; then
    mkdir -p $RELEASE_DIR
    cd $RELEASE_DIR

    cmake -C $ROOT_DIR/poco.cmake -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build . --config Release -j $(nproc)

    cd $ROOT_DIR
elif [ "$1" = "all" ]; then
    mkdir -p $DEBUG_DIR
    cd $DEBUG_DIR

    cmake -C $ROOT_DIR/poco.cmake -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build . --config Debug -j $(nproc)
    echo ""
    echo ""
    echo ""

    cd $ROOT_DIR

    mkdir -p $RELEASE_DIR
    cd $RELEASE_DIR

    cmake -C $ROOT_DIR/poco.cmake -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build . --config Release -j $(nproc)

    cd $ROOT_DIR
else
    echo "Usage: ./build.poco.sh [debug | release | all]"
fi
