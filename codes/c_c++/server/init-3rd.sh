#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
POCO_INIT_DIR=$ROOT_INIT_DIR/3rd/poco
LIBEVENT_INIT_DIR=$ROOT_INIT_DIR/3rd/libevent
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
    git checkout devel
fi

echo ------------------
echo -- libevent
cd $ROOT_INIT_DIR
if [ ! -d $LIBEVENT_INIT_DIR ]; then
    git clone -b release-2.1.12-stable https://github.com/libevent/libevent.git $LIBEVENT_INIT_DIR
else
    cd $LIBEVENT_INIT_DIR
    git checkout .
    git fetch -p origin
    git checkout release-2.1.12-stable
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
    git checkout 3.19.x
fi

echo ------------------
echo -- googletest
cd $ROOT_INIT_DIR
if [ ! -d $GOOGLETEST_INIT_DIR ]; then
    git clone -b release-1.12.1 https://github.com/google/googletest.git $GOOGLETEST_INIT_DIR
else
    cd $GOOGLETEST_INIT_DIR
    git checkout .
    git fetch -p origin
    git checkout release-1.12.1
fi
