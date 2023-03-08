#!/bin/bash

ROOT_INIT_DIR=`pwd`
ASIO_INIT_DIR=$ROOT_INIT_DIR/3rd/asio
COMMON_INIT_DIR=$ROOT_INIT_DIR/3rd/CppCommon
SERVER_INIT_DIR=$ROOT_INIT_DIR/3rd/CppServer

cd $ROOT_INIT_DIR
if [ ! -d $ASIO_INIT_DIR ]; then
    git clone -b asio-1-12-branch https://github.com/chriskohlhoff/asio.git $ASIO_INIT_DIR

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
