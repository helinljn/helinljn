#!/bin/bash

ROOT_DIR=`pwd`
BUILD_DIR=$ROOT_DIR/3rd/3rd-cmake

if   [ "$1" = "debug" ]; then
    cd $BUILD_DIR

    echo ""
    echo "--------------------------------"
    echo "-- Building poco..."
    chmod +x poco.build.sh
    ./poco.build.sh debug

    echo ""
    echo "--------------------------------"
    echo "-- Building curl..."
    chmod +x curl.build.sh
    ./curl.build.sh debug

    echo ""
    echo "--------------------------------"
    echo "-- Building libhv..."
    chmod +x libhv.build.sh
    ./libhv.build.sh debug

    cd $ROOT_DIR
elif [ "$1" = "release" ]; then
    cd $BUILD_DIR

    echo ""
    echo "--------------------------------"
    echo "-- Building poco..."
    chmod +x poco.build.sh
    ./poco.build.sh release

    echo ""
    echo "--------------------------------"
    echo "-- Building curl..."
    chmod +x curl.build.sh
    ./curl.build.sh release

    echo ""
    echo "--------------------------------"
    echo "-- Building libhv..."
    chmod +x libhv.build.sh
    ./libhv.build.sh release

    cd $ROOT_DIR
elif [ "$1" = "all" ]; then
    cd $BUILD_DIR

    echo ""
    echo "--------------------------------"
    echo "-- Building poco..."
    chmod +x poco.build.sh
    ./poco.build.sh all

    echo ""
    echo "--------------------------------"
    echo "-- Building curl..."
    chmod +x curl.build.sh
    ./curl.build.sh all

    echo ""
    echo "--------------------------------"
    echo "-- Building libhv..."
    chmod +x libhv.build.sh
    ./libhv.build.sh all

    cd $ROOT_DIR
else
    echo "Usage: ./build-3rd.sh [debug | release | all]"
fi
