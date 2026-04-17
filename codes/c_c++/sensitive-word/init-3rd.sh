#!/bin/bash

ROOT_INIT_DIR=`pwd`
BRYNET_INIT_DIR=$ROOT_INIT_DIR/3rd/brynet
SPDLOG_INIT_DIR=$ROOT_INIT_DIR/3rd/spdlog
DOCTEST_INIT_DIR=$ROOT_INIT_DIR/3rd/doctest

echo ------------------
echo -- brynet
cd $ROOT_INIT_DIR
if [ ! -d $BRYNET_INIT_DIR ]; then
    git clone https://github.com/IronsDu/brynet.git $BRYNET_INIT_DIR
else
    cd $BRYNET_INIT_DIR
    git remote set-url origin https://github.com/IronsDu/brynet.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
fi

echo ------------------
echo -- spdlog
cd $ROOT_INIT_DIR
if [ ! -d $SPDLOG_INIT_DIR ]; then
    git clone -b v1.15.3 https://github.com/gabime/spdlog.git $SPDLOG_INIT_DIR
else
    cd $SPDLOG_INIT_DIR
    git remote set-url origin https://github.com/gabime/spdlog.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v1.15.3
fi

echo ------------------
echo -- doctest
cd $ROOT_INIT_DIR
if [ ! -d $DOCTEST_INIT_DIR ]; then
    git clone -b v2.4.12 https://github.com/doctest/doctest.git $DOCTEST_INIT_DIR
else
    cd $DOCTEST_INIT_DIR
    git remote set-url origin https://github.com/doctest/doctest.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v2.4.12
fi
