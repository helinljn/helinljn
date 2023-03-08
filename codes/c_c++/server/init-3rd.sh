#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
ASIO_INIT_DIR=$ROOT_INIT_DIR/3rd/asio
SPDLOG_INIT_DIR=$ROOT_INIT_DIR/3rd/spdlog
TINYXML2_INIT_DIR=$ROOT_INIT_DIR/3rd/tinyxml2
JSONCPP_INIT_DIR=$ROOT_INIT_DIR/3rd/jsoncpp
CPP_COMMON_INIT_DIR=$ROOT_INIT_DIR/3rd/CppCommon
CPP_SERVER_INIT_DIR=$ROOT_INIT_DIR/3rd/CppServer

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
if [ ! -d $SPDLOG_INIT_DIR ]; then
    git clone -b v1.11.0 https://github.com/gabime/spdlog.git $SPDLOG_INIT_DIR

    cd $SPDLOG_INIT_DIR
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $TINYXML2_INIT_DIR ]; then
    git clone -b 9.0.0 https://github.com/leethomason/tinyxml2.git $TINYXML2_INIT_DIR

    cd $TINYXML2_INIT_DIR
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $JSONCPP_INIT_DIR ]; then
    git clone https://github.com/open-source-parsers/jsoncpp.git $JSONCPP_INIT_DIR
else
    cd $JSONCPP_INIT_DIR
    git pull
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $CPP_COMMON_INIT_DIR ]; then
    git clone https://github.com/chronoxor/CppCommon.git $CPP_COMMON_INIT_DIR
else
    cd $CPP_COMMON_INIT_DIR
    git pull
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $CPP_SERVER_INIT_DIR ]; then
    git clone https://github.com/chronoxor/CppServer.git $CPP_SERVER_INIT_DIR
else
    cd $CPP_SERVER_INIT_DIR
    git pull
    git log -1
fi
