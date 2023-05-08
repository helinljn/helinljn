#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
ZLIB_INIT_DIR=$ROOT_INIT_DIR/3rd/zlib
SPDLOG_INIT_DIR=$ROOT_INIT_DIR/3rd/spdlog
TINYXML2_INIT_DIR=$ROOT_INIT_DIR/3rd/tinyxml2
HIREDIS_INIT_DIR=$ROOT_INIT_DIR/3rd/hiredis
LIBEVENT_INIT_DIR=$ROOT_INIT_DIR/3rd/libevent
PROTOBUF_INIT_DIR=$ROOT_INIT_DIR/3rd/protobuf
GOOGLETEST_INIT_DIR=$ROOT_INIT_DIR/3rd/googletest
JSONCPP_INIT_DIR=$ROOT_INIT_DIR/3rd/jsoncpp

cd $ROOT_INIT_DIR
if [ ! -d $FMT_INIT_DIR ]; then
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git $FMT_INIT_DIR

    cd $FMT_INIT_DIR
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $ZLIB_INIT_DIR ]; then
    git clone -b v1.2.13 https://github.com/madler/zlib.git $ZLIB_INIT_DIR

    cd $ZLIB_INIT_DIR
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
if [ ! -d $HIREDIS_INIT_DIR ]; then
    git clone -b v1.0.2 https://github.com/redis/hiredis.git $HIREDIS_INIT_DIR

    cd $HIREDIS_INIT_DIR
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $LIBEVENT_INIT_DIR ]; then
    git clone -b release-2.1.12-stable https://github.com/libevent/libevent.git $LIBEVENT_INIT_DIR

    cd $LIBEVENT_INIT_DIR
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $PROTOBUF_INIT_DIR ]; then
    git clone -b 3.18.x https://github.com/protocolbuffers/protobuf.git $PROTOBUF_INIT_DIR

    cd $PROTOBUF_INIT_DIR
    git log -1
fi

cd $ROOT_INIT_DIR
if [ ! -d $GOOGLETEST_INIT_DIR ]; then
    git clone -b release-1.12.1 https://github.com/google/googletest.git $GOOGLETEST_INIT_DIR

    cd $GOOGLETEST_INIT_DIR
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
