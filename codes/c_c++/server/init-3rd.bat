@echo off

set ROOT_INIT_DIR=%~dp0
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set POCO_INIT_DIR=%ROOT_INIT_DIR%/3rd/poco
set LIBEVENT_INIT_DIR=%ROOT_INIT_DIR%/3rd/libevent
set PROTOBUF_INIT_DIR=%ROOT_INIT_DIR%/3rd/protobuf
set GOOGLETEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/googletest

echo ------------------
echo -- fmt
cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git %FMT_INIT_DIR%
) else (
    cd %FMT_INIT_DIR%
    git checkout .
    git fetch -p origin
    git checkout 9.1.0
)

echo ------------------
echo -- poco
cd %ROOT_INIT_DIR%
if not exist %POCO_INIT_DIR% (
    git clone -b devel https://github.com/pocoproject/poco.git %POCO_INIT_DIR%
) else (
    cd %POCO_INIT_DIR%
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- libevent
cd %ROOT_INIT_DIR%
if not exist %LIBEVENT_INIT_DIR% (
    git clone -b patches-2.1 https://github.com/libevent/libevent.git %LIBEVENT_INIT_DIR%
) else (
    cd %LIBEVENT_INIT_DIR%
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- protobuf
cd %ROOT_INIT_DIR%
if not exist %PROTOBUF_INIT_DIR% (
    git clone -b 3.19.x https://github.com/protocolbuffers/protobuf.git %PROTOBUF_INIT_DIR%
) else (
    cd %PROTOBUF_INIT_DIR%
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- googletest
cd %ROOT_INIT_DIR%
if not exist %GOOGLETEST_INIT_DIR% (
    git clone -b v1.12.x https://github.com/google/googletest.git %GOOGLETEST_INIT_DIR%
) else (
    cd %GOOGLETEST_INIT_DIR%
    git checkout .
    git fetch -p origin
    git pull
)

pause