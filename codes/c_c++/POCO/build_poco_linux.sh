#!/bin/bash

POCO_INIT_DIR=`pwd`/poco
POCO_OPENSSL_DIR=$POCO_INIT_DIR/../../vcpkg/vcpkg/packages/openssl_x64-linux/include
POCO_BRANCH_NAME=poco-1.12.4-release

if [ ! -d $POCO_INIT_DIR ]; then
    git clone https://github.com/pocoproject/poco.git $POCO_INIT_DIR
fi

cd $POCO_INIT_DIR

git checkout $POCO_BRANCH_NAME

./configure --include-path=$POCO_OPENSSL_DIR --no-tests --no-samples --static --omit=CppUnit,Encodings,NetSSL_Win,\
Data,Data/SQLite,Data/ODBC,Data/MySQL,Data/PostgreSQL,Zip,PageCompiler,PageCompiler/File2Page,PDF,CppParser,MongoDB,\
Prometheus,ActiveRecord,ActiveRecord/Compiler,PocoDoc,ProGen

make -j `nproc`

mkdir -p $POCO_INIT_DIR/lib64

cp -nf $POCO_INIT_DIR/lib/Linux/x86_64/*.a $POCO_INIT_DIR/lib64
