#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
ASIO_INIT_DIR=$ROOT_INIT_DIR/3rd/asio
COMMON_INIT_DIR=$ROOT_INIT_DIR/3rd/CppCommon
SERVER_INIT_DIR=$ROOT_INIT_DIR/3rd/CppServer

cd $ROOT_INIT_DIR
if [ ! -d $FMT_INIT_DIR ]; then
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git $FMT_INIT_DIR

    cd $FMT_INIT_DIR
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $ASIO_INIT_DIR ]; then
    git clone -b asio-1-22-2 https://github.com/chriskohlhoff/asio.git $ASIO_INIT_DIR

    cd $ASIO_INIT_DIR
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $COMMON_INIT_DIR ]; then
    git clone https://github.com/chronoxor/CppCommon.git $COMMON_INIT_DIR
else
    cd $COMMON_INIT_DIR
    git pull
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $SERVER_INIT_DIR ]; then
    git clone https://github.com/chronoxor/CppServer.git $SERVER_INIT_DIR
else
    cd $SERVER_INIT_DIR
    git pull
    git log -1
fi
