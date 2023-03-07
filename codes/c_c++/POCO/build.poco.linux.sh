#!/bin/bash

POCO_INIT_DIR=`pwd`/poco
POCO_OPENSSL_DIR=$POCO_INIT_DIR/../../3rd-libs/openssl-1.1.1n/x64-ubuntu-20.04/include

if [ ! -d $POCO_INIT_DIR ]; then
    git clone https://github.com/pocoproject/poco.git $POCO_INIT_DIR
fi

cd $POCO_INIT_DIR

git checkout poco-1.12.4-release

./configure --include-path=$POCO_OPENSSL_DIR --no-tests --no-samples --static

make Foundation-libexec -j `nproc`
make XML-libexec -j `nproc`
make JSON-libexec -j `nproc`
make Util-libexec -j `nproc`
make Net-libexec -j `nproc`
make Crypto-libexec -j `nproc`
make NetSSL_OpenSSL-libexec -j `nproc`
make Zip-libexec -j `nproc`
make JWT-libexec -j `nproc`

mkdir -p $POCO_INIT_DIR/lib64
cp -nf $POCO_INIT_DIR/lib/Linux/x86_64/*.a $POCO_INIT_DIR/lib64