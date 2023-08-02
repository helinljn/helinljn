#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
LIBEVENT_INIT_DIR=$ROOT_INIT_DIR/3rd/libevent
PROTOBUF_INIT_DIR=$ROOT_INIT_DIR/3rd/protobuf
POCO_INIT_DIR=$ROOT_INIT_DIR/3rd/poco

cd $ROOT_INIT_DIR
if [ ! -d $FMT_INIT_DIR ]; then
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git $FMT_INIT_DIR
else
    cd $FMT_INIT_DIR
    git checkout .
fi

cd $ROOT_INIT_DIR
if [ ! -d $LIBEVENT_INIT_DIR ]; then
    git clone -b release-2.1.12-stable https://github.com/libevent/libevent.git $LIBEVENT_INIT_DIR
else
    cd $LIBEVENT_INIT_DIR
    git checkout .
fi

cd $ROOT_INIT_DIR
if [ ! -d $PROTOBUF_INIT_DIR ]; then
    git clone -b 3.18.x https://github.com/protocolbuffers/protobuf.git $PROTOBUF_INIT_DIR
else
    cd $PROTOBUF_INIT_DIR
    git checkout .
fi

cd $ROOT_INIT_DIR
if [ ! -d $POCO_INIT_DIR ]; then
    git clone https://github.com/pocoproject/poco.git $POCO_INIT_DIR
else
    cd $POCO_INIT_DIR
    git checkout .
    git pull
fi
