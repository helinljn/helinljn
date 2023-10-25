#!/bin/bash

PBMSG_DIR=../../src/common/pbmsg

mkdir -p $PBMSG_DIR

find $PBMSG_DIR -name "*.proto" | xargs ./protoc-exec --proto_path=$PBMSG_DIR --cpp_out=$PBMSG_DIR
find $PBMSG_DIR -name '*.pb.h'  | xargs unix2dos
find $PBMSG_DIR -name '*.pb.cc' | xargs unix2dos