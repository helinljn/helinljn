#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
SPDLOG_INIT_DIR=$ROOT_INIT_DIR/3rd/spdlog
BRYNET_INIT_DIR=$ROOT_INIT_DIR/3rd/brynet

echo ------------------
echo -- fmt
cd $ROOT_INIT_DIR
if [ ! -d $FMT_INIT_DIR ]; then
    git clone -b 9.1.0 https://hub.yzuu.cf/fmtlib/fmt.git $FMT_INIT_DIR
else
    cd $FMT_INIT_DIR
    git remote set-url origin https://hub.yzuu.cf/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 9.1.0
fi

echo ------------------
echo -- spdlog
cd $ROOT_INIT_DIR
if [ ! -d $SPDLOG_INIT_DIR ]; then
    git clone -b v1.11.0 https://hub.yzuu.cf/gabime/spdlog.git $SPDLOG_INIT_DIR
else
    cd $SPDLOG_INIT_DIR
    git remote set-url origin https://hub.yzuu.cf/gabime/spdlog.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v1.11.0
fi

echo ------------------
echo -- brynet
cd $ROOT_INIT_DIR
if [ ! -d $BRYNET_INIT_DIR ]; then
    git clone https://hub.yzuu.cf/IronsDu/brynet.git $BRYNET_INIT_DIR
else
    cd $BRYNET_INIT_DIR
    git remote set-url origin https://hub.yzuu.cf/IronsDu/brynet.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
fi
