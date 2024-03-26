#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
QUILL_INIT_DIR=$ROOT_INIT_DIR/3rd/quill
BRYNET_INIT_DIR=$ROOT_INIT_DIR/3rd/brynet

echo ------------------
echo -- fmt
cd $ROOT_INIT_DIR
if [ ! -d $FMT_INIT_DIR ]; then
    git clone -b 10.2.1 https://github.hscsec.cn/fmtlib/fmt.git $FMT_INIT_DIR
else
    cd $FMT_INIT_DIR
    git remote set-url origin https://github.hscsec.cn/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 10.2.1
fi

echo ------------------
echo -- quill
cd $ROOT_INIT_DIR
if [ ! -d $QUILL_INIT_DIR ]; then
    git clone -b v3.8.0 https://github.hscsec.cn/odygrd/quill.git $QUILL_INIT_DIR
else
    cd $QUILL_INIT_DIR
    git remote set-url origin https://github.hscsec.cn/odygrd/quill.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v3.8.0
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
