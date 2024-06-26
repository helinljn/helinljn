#!/bin/bash

PBMSG_DIR=../../src/common/pbmsg
KERNEL_VERSION=$(uname -r)

mkdir -p $PBMSG_DIR

find $PBMSG_DIR -name "*.proto" | xargs ./protoc-exec --proto_path=$PBMSG_DIR --cpp_out=dllexport_decl=PROTOC_EXPORT:$PBMSG_DIR

if [[ ${KERNEL_VERSION,,} == *microsoft* ]]; then
    find $PBMSG_DIR -name '*.pb.h'  | xargs unix2dos
    find $PBMSG_DIR -name '*.pb.cc' | xargs unix2dos
fi
