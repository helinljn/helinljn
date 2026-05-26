#!/bin/bash

ROOT_INIT_DIR=`pwd`
POCO_INIT_DIR=$ROOT_INIT_DIR/3rd/poco
CURL_INIT_DIR=$ROOT_INIT_DIR/3rd/curl
DOCTEST_INIT_DIR=$ROOT_INIT_DIR/3rd/doctest
MIMALLOC_INIT_DIR=$ROOT_INIT_DIR/3rd/mimalloc

echo ------------------
echo -- poco
cd $ROOT_INIT_DIR
if [ ! -d $POCO_INIT_DIR ]; then
    git clone -b poco-1.15.3-release https://github.com/pocoproject/poco.git $POCO_INIT_DIR
else
    cd $POCO_INIT_DIR
    git remote set-url origin https://github.com/pocoproject/poco.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout poco-1.15.3-release
fi

echo ------------------
echo -- curl
cd $ROOT_INIT_DIR
if [ ! -d $CURL_INIT_DIR ]; then
    git clone -b curl-8_20_0 https://github.com/curl/curl.git $CURL_INIT_DIR
else
    cd $CURL_INIT_DIR
    git remote set-url origin https://github.com/curl/curl.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout curl-8_20_0
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
echo -- mimalloc
cd $ROOT_INIT_DIR
if [ ! -d $MIMALLOC_INIT_DIR ]; then
    git clone -b v2.3.2 https://github.com/microsoft/mimalloc.git $MIMALLOC_INIT_DIR
else
    cd $MIMALLOC_INIT_DIR
    git remote set-url origin https://github.com/microsoft/mimalloc.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v2.3.2
fi
