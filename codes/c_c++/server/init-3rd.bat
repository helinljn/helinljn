@echo off

set ROOT_INIT_DIR=%CD%
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set ZLIB_INIT_DIR=%ROOT_INIT_DIR%/3rd/zlib
set SPDLOG_INIT_DIR=%ROOT_INIT_DIR%/3rd/spdlog
set TINYXML2_INIT_DIR=%ROOT_INIT_DIR%/3rd/tinyxml2
set HIREDIS_INIT_DIR=%ROOT_INIT_DIR%/3rd/hiredis
set LIBEVENT_INIT_DIR=%ROOT_INIT_DIR%/3rd/libevent
set PROTOBUF_INIT_DIR=%ROOT_INIT_DIR%/3rd/protobuf
set GOOGLETEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/googletest
set JSONCPP_INIT_DIR=%ROOT_INIT_DIR%/3rd/jsoncpp

cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git %FMT_INIT_DIR%
) else (
    cd %FMT_INIT_DIR%
    git checkout .
)

cd %ROOT_INIT_DIR%
if not exist %ZLIB_INIT_DIR% (
    git clone -b v1.2.13 https://github.com/madler/zlib.git %ZLIB_INIT_DIR%
) else (
    cd %ZLIB_INIT_DIR%
    git checkout .
)

cd %ROOT_INIT_DIR%
if not exist %SPDLOG_INIT_DIR% (
    git clone -b v1.11.0 https://github.com/gabime/spdlog.git %SPDLOG_INIT_DIR%
) else (
    cd %SPDLOG_INIT_DIR%
    git checkout .
)

cd %ROOT_INIT_DIR%
if not exist %TINYXML2_INIT_DIR% (
    git clone -b 9.0.0 https://github.com/leethomason/tinyxml2.git %TINYXML2_INIT_DIR%
) else (
    cd %TINYXML2_INIT_DIR%
    git checkout .
)

cd %ROOT_INIT_DIR%
if not exist %HIREDIS_INIT_DIR% (
    git clone -b v1.0.2 https://github.com/redis/hiredis.git %HIREDIS_INIT_DIR%
) else (
    cd %HIREDIS_INIT_DIR%
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

cd %ROOT_INIT_DIR%
if not exist %GOOGLETEST_INIT_DIR% (
    git clone -b release-1.12.1 https://github.com/google/googletest.git %GOOGLETEST_INIT_DIR%
) else (
    cd %GOOGLETEST_INIT_DIR%
    git checkout .
)

cd %ROOT_INIT_DIR%
if not exist %JSONCPP_INIT_DIR% (
    git clone https://github.com/open-source-parsers/jsoncpp.git %JSONCPP_INIT_DIR%
) else (
    cd %JSONCPP_INIT_DIR%
    git checkout .
    git pull
)

pause