#!/bin/bash

ROOT_DIR=`pwd`
DEBUG_DIR=$ROOT_DIR/.build/Debug
RELEASE_DIR=$ROOT_DIR/.build/Release
EXECUTABLE_LIST="./test ./libssl.so ./libcrypto.so ./libmysqlclient.so"

if   [ "$1" = "debug" ]; then
    mkdir -p $DEBUG_DIR
    cd $DEBUG_DIR

    for item in $EXECUTABLE_LIST
    do
        patchelf --set-rpath ./ $item
        temp=$(readelf -d $item | grep RUNPATH | awk -F ')' '{print $2}')
        printf "%-20s%s\n" "$item" "$temp"
    done

    cd $ROOT_DIR
elif [ "$1" = "release" ]; then
    mkdir -p $RELEASE_DIR
    cd $RELEASE_DIR

    for item in $EXECUTABLE_LIST
    do
        patchelf --set-rpath ./ $item
        temp=$(readelf -d $item | grep RUNPATH | awk -F ')' '{print $2}')
        printf "%-20s%s\n" "$item" "$temp"
    done

    cd $ROOT_DIR
elif [ "$1" = "all" ]; then
    mkdir -p $DEBUG_DIR
    cd $DEBUG_DIR

    for item in $EXECUTABLE_LIST
    do
        patchelf --set-rpath ./ $item
        temp=$(readelf -d $item | grep RUNPATH | awk -F ')' '{print $2}')
        printf "%-20s%s\n" "$item" "$temp"
    done
    echo ""
    echo ""
    echo ""

    cd $ROOT_DIR

    mkdir -p $RELEASE_DIR
    cd $RELEASE_DIR

    for item in $EXECUTABLE_LIST
    do
        patchelf --set-rpath ./ $item
        temp=$(readelf -d $item | grep RUNPATH | awk -F ')' '{print $2}')
        printf "%-20s%s\n" "$item" "$temp"
    done

    cd $ROOT_DIR
else
    echo "Usage: ./set-rpath.sh [debug | release | all]"
fi
