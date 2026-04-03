#!/bin/bash

ROOT_INIT_DIR=`pwd`
FMT_INIT_DIR=$ROOT_INIT_DIR/3rd/fmt
SPDLOG_INIT_DIR=$ROOT_INIT_DIR/3rd/spdlog
DOCTEST_INIT_DIR=$ROOT_INIT_DIR/3rd/doctest
SIMPLEINI_INIT_DIR=$ROOT_INIT_DIR/3rd/simpleini
JSONCPP_INIT_DIR=$ROOT_INIT_DIR/3rd/jsoncpp
TINYXML2_INIT_DIR=$ROOT_INIT_DIR/3rd/tinyxml2
BRYNET_INIT_DIR=$ROOT_INIT_DIR/3rd/brynet
LIGHTHOOK_INIT_DIR=$ROOT_INIT_DIR/3rd/LightHook
DISTORM_INIT_DIR=$ROOT_INIT_DIR/3rd/distorm
FUNCHOOK_INIT_DIR=$ROOT_INIT_DIR/3rd/funchook

echo ------------------
echo -- fmt
cd $ROOT_INIT_DIR
if [ ! -d $FMT_INIT_DIR ]; then
    git clone -b 11.2.0 https://github.com/fmtlib/fmt.git $FMT_INIT_DIR
else
    cd $FMT_INIT_DIR
    git remote set-url origin https://github.com/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 11.2.0
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

echo ------------------
echo -- simpleini
cd $ROOT_INIT_DIR
if [ ! -d $SIMPLEINI_INIT_DIR ]; then
    git clone -b v4.25 https://github.com/brofield/simpleini.git $SIMPLEINI_INIT_DIR
else
    cd $SIMPLEINI_INIT_DIR
    git remote set-url origin https://github.com/brofield/simpleini.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v4.25
fi

echo ------------------
echo -- jsoncpp
cd $ROOT_INIT_DIR
if [ ! -d $JSONCPP_INIT_DIR ]; then
    git clone -b 1.9.7 https://github.com/open-source-parsers/jsoncpp.git $JSONCPP_INIT_DIR
else
    cd $JSONCPP_INIT_DIR
    git remote set-url origin https://github.com/open-source-parsers/jsoncpp.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 1.9.7
fi

echo ------------------
echo -- tinyxml2
cd $ROOT_INIT_DIR
if [ ! -d $TINYXML2_INIT_DIR ]; then
    git clone -b 10.1.0 https://github.com/leethomason/tinyxml2.git $TINYXML2_INIT_DIR
else
    cd $TINYXML2_INIT_DIR
    git remote set-url origin https://github.com/leethomason/tinyxml2.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 10.1.0
fi

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
echo -- LightHook
cd $ROOT_INIT_DIR
if [ ! -d $LIGHTHOOK_INIT_DIR ]; then
    git clone https://github.com/SamuelTulach/LightHook.git $LIGHTHOOK_INIT_DIR
else
    cd $LIGHTHOOK_INIT_DIR
    git remote set-url origin https://github.com/SamuelTulach/LightHook.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
fi

echo ------------------
echo -- distorm
cd $ROOT_INIT_DIR
if [ ! -d $DISTORM_INIT_DIR ]; then
    git clone https://github.com/gdabah/distorm.git $DISTORM_INIT_DIR
else
    cd $DISTORM_INIT_DIR
    git remote set-url origin https://github.com/gdabah/distorm.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
fi

echo ------------------
echo -- funchook
cd $ROOT_INIT_DIR
if [ ! -d $FUNCHOOK_INIT_DIR ]; then
    git clone https://github.com/kubo/funchook.git $FUNCHOOK_INIT_DIR
else
    cd $FUNCHOOK_INIT_DIR
    git remote set-url origin https://github.com/kubo/funchook.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
fi
