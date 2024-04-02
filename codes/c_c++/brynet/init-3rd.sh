#!/bin/bash

ROOT_INIT_DIR=`pwd`
SZ_INIT_DIR=$ROOT_INIT_DIR/3rd/sz
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
QUILL_INIT_DIR=$ROOT_INIT_DIR/3rd/quill
BRYNET_INIT_DIR=$ROOT_INIT_DIR/3rd/brynet
GOOGLETEST_INIT_DIR=$ROOT_INIT_DIR/3rd/googletest

echo ------------------
echo -- sz
cd $ROOT_INIT_DIR
if [ ! -d $SZ_INIT_DIR ]; then
    git clone -b v3.7.1 https://kkgithub.com/ashvardanian/StringZilla.git $SZ_INIT_DIR
else
    cd $SZ_INIT_DIR
    git remote set-url origin https://kkgithub.com/ashvardanian/StringZilla.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v3.7.1
fi

echo ------------------
echo -- fmt
cd $ROOT_INIT_DIR
if [ ! -d $FMT_INIT_DIR ]; then
    git clone -b 10.2.1 https://kkgithub.com/fmtlib/fmt.git $FMT_INIT_DIR
else
    cd $FMT_INIT_DIR
    git remote set-url origin https://kkgithub.com/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 10.2.1
fi

echo ------------------
echo -- quill
cd $ROOT_INIT_DIR
if [ ! -d $QUILL_INIT_DIR ]; then
    git clone -b v3.8.0 https://kkgithub.com/odygrd/quill.git $QUILL_INIT_DIR
else
    cd $QUILL_INIT_DIR
    git remote set-url origin https://kkgithub.com/odygrd/quill.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v3.8.0
fi

echo ------------------
echo -- brynet
cd $ROOT_INIT_DIR
if [ ! -d $BRYNET_INIT_DIR ]; then
    git clone https://kkgithub.com/IronsDu/brynet.git $BRYNET_INIT_DIR
else
    cd $BRYNET_INIT_DIR
    git remote set-url origin https://kkgithub.com/IronsDu/brynet.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
fi

echo ------------------
echo -- googletest
cd $ROOT_INIT_DIR
if [ ! -d $GOOGLETEST_INIT_DIR ]; then
    git clone -b v1.14.x https://kkgithub.com/google/googletest.git $GOOGLETEST_INIT_DIR
else
    cd $GOOGLETEST_INIT_DIR
    git remote set-url origin https://kkgithub.com/google/googletest.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
fi
