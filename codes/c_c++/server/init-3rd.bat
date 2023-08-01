@echo off

set ROOT_INIT_DIR=%CD%
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set POCO_INIT_DIR=%ROOT_INIT_DIR%/3rd/poco
set LIBEVENT_INIT_DIR=%ROOT_INIT_DIR%/3rd/libevent
set PROTOBUF_INIT_DIR=%ROOT_INIT_DIR%/3rd/protobuf

cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git %FMT_INIT_DIR%
) else (
    cd %FMT_INIT_DIR%
    git checkout .
)

cd %ROOT_INIT_DIR%
if not exist %POCO_INIT_DIR% (
    git clone -b poco-1.12.4-release https://github.com/pocoproject/poco.git %POCO_INIT_DIR%
) else (
    cd %POCO_INIT_DIR%
    git checkout .
)

cd %ROOT_INIT_DIR%
if not exist %LIBEVENT_INIT_DIR% (
    git clone -b release-2.1.12-stable https://github.com/libevent/libevent.git %LIBEVENT_INIT_DIR%
) else (
    cd %LIBEVENT_INIT_DIR%
    git checkout .
)

cd %ROOT_INIT_DIR%
if not exist %PROTOBUF_INIT_DIR% (
    git clone -b 3.18.x https://github.com/protocolbuffers/protobuf.git %PROTOBUF_INIT_DIR%
) else (
    cd %PROTOBUF_INIT_DIR%
    git checkout .
)

pause