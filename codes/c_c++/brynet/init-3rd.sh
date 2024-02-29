#!/bin/bash

ROOT_INIT_DIR=`pwd`
SPDLOG_INIT_DIR=$ROOT_INIT_DIR/3rd/spdlog
BRYNET_INIT_DIR=$ROOT_INIT_DIR/3rd/brynet

echo ------------------
echo -- spdlog
cd $ROOT_INIT_DIR
if [ ! -d $SPDLOG_INIT_DIR ]; then
    git clone -b v1.13.0 https://github.hscsec.cn/gabime/spdlog.git $SPDLOG_INIT_DIR
else
    cd $SPDLOG_INIT_DIR
    git remote set-url origin https://github.hscsec.cn/gabime/spdlog.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v1.13.0
fi

echo ------------------
echo -- brynet
cd $ROOT_INIT_DIR
if [ ! -d $BRYNET_INIT_DIR ]; then
    git clone https://github.hscsec.cn/IronsDu/brynet.git $BRYNET_INIT_DIR
else
    cd $BRYNET_INIT_DIR
    git remote set-url origin https://github.hscsec.cn/IronsDu/brynet.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
fi
