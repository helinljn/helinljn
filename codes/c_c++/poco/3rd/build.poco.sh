#!/bin/bash

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
DEBUG_DIR="$ROOT_DIR/poco/.build/linux/x64-Debug"
RELEASE_DIR="$ROOT_DIR/poco/.build/linux/x64-Release"
POCO_MODULES=(Foundation Encodings XML JSON Util Net NetSSL Crypto Data DataSQLite DataMySQL Redis JWT Prometheus Zip)

build_poco_config() {
    local POCO_CONFIG=$1
    local POCO_BUILD_DIR=$2

    mkdir -p "$POCO_BUILD_DIR" || exit 1
    cd "$POCO_BUILD_DIR" || exit 1

    cmake -C "$ROOT_DIR/poco.cmake" -DCMAKE_BUILD_TYPE="$POCO_CONFIG" ../../../
    if [ $? -ne 0 ]; then exit 1; fi

    cmake --build . --config "$POCO_CONFIG" --parallel 16
    if [ $? -ne 0 ]; then exit 1; fi

    install_poco_config "$POCO_CONFIG" "$POCO_BUILD_DIR" || exit 1

    cd "$ROOT_DIR" || exit 1
}

install_poco_config() {
    local POCO_CONFIG=$1
    local POCO_BUILD_DIR=$2
    local POCO_BIN_DIR="$POCO_BUILD_DIR/bin"
    local POCO_LIB_DIR="$POCO_BUILD_DIR/lib"
    local POCO_SUFFIX=

    if [ "$POCO_CONFIG" = "Debug" ]; then
        POCO_SUFFIX=d
    fi

    mkdir -p "$POCO_LIB_DIR" || return 1

    for module in "${POCO_MODULES[@]}"; do
        copy_required "$POCO_BIN_DIR/libPoco${module}${POCO_SUFFIX}.so" "$POCO_LIB_DIR" || return 1
        copy_required "$POCO_BIN_DIR/libPoco${module}${POCO_SUFFIX}.so.123" "$POCO_LIB_DIR" || return 1
    done
}

copy_required() {
    local POCO_SOURCE_FILE=$1
    local POCO_TARGET_DIR=$2
    local POCO_TARGET_FILE="$POCO_TARGET_DIR/$(basename "$POCO_SOURCE_FILE")"

    if [ ! -e "$POCO_SOURCE_FILE" ] && [ ! -L "$POCO_SOURCE_FILE" ]; then
        echo "Missing Poco build artifact: $POCO_SOURCE_FILE"
        return 1
    fi

    if [ -L "$POCO_SOURCE_FILE" ]; then
        local POCO_LINK_TARGET
        POCO_LINK_TARGET=$(readlink "$POCO_SOURCE_FILE") || return 1
        rm -f "$POCO_TARGET_FILE" || return 1
        ln -s "$POCO_LINK_TARGET" "$POCO_TARGET_FILE" || return 1
    else
        cp -f "$POCO_SOURCE_FILE" "$POCO_TARGET_FILE" || return 1
    fi
}

if [ "$1" = "debug" ]; then
    build_poco_config Debug "$DEBUG_DIR"
elif [ "$1" = "release" ]; then
    build_poco_config Release "$RELEASE_DIR"
elif [ "$1" = "all" ]; then
    build_poco_config Debug "$DEBUG_DIR"
    echo ""
    echo ""
    echo ""

    build_poco_config Release "$RELEASE_DIR"
else
    echo "Usage: ./build.poco.sh [debug | release | all]"
fi
