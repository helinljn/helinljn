#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
POCO_INIT_DIR=$ROOT_INIT_DIR/3rd/poco
PROTOBUF_INIT_DIR=$ROOT_INIT_DIR/3rd/protobuf
GOOGLETEST_INIT_DIR=$ROOT_INIT_DIR/3rd/googletest

echo ------------------
echo -- fmt
cd $ROOT_INIT_DIR
if [ ! -d $FMT_INIT_DIR ]; then
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git $FMT_INIT_DIR
else
    cd $FMT_INIT_DIR
    git checkout .
    git fetch -p origin
    git checkout 9.1.0
fi

echo ------------------
echo -- poco
cd $ROOT_INIT_DIR
if [ ! -d $POCO_INIT_DIR ]; then
    git clone -b devel https://github.com/pocoproject/poco.git $POCO_INIT_DIR
else
    cd $POCO_INIT_DIR
    git checkout .
    git fetch -p origin
    git pull
fi

echo ------------------
echo -- protobuf
cd $ROOT_INIT_DIR
if [ ! -d $PROTOBUF_INIT_DIR ]; then
    git clone -b 3.19.x https://github.com/protocolbuffers/protobuf.git $PROTOBUF_INIT_DIR
else
    cd $PROTOBUF_INIT_DIR
    git checkout .
    git fetch -p origin
    git pull
fi

echo ------------------
echo -- googletest
cd $ROOT_INIT_DIR
if [ ! -d $GOOGLETEST_INIT_DIR ]; then
    git clone -b v1.12.x https://github.com/google/googletest.git $GOOGLETEST_INIT_DIR
else
    cd $GOOGLETEST_INIT_DIR
    git checkout .
    git fetch -p origin
    git pull
fi
