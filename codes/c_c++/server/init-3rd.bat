@echo off

set ROOT_INIT_DIR=%CD%
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set POCO_INIT_DIR=%ROOT_INIT_DIR%/3rd/poco
set SPDLOG_INIT_DIR=%ROOT_INIT_DIR%/3rd/spdlog
set HIREDIS_INIT_DIR=%ROOT_INIT_DIR%/3rd/hiredis
set LIBEVENT_INIT_DIR=%ROOT_INIT_DIR%/3rd/libevent
set PROTOBUF_INIT_DIR=%ROOT_INIT_DIR%/3rd/protobuf
set GOOGLETEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/googletest
set JSONCPP_INIT_DIR=%ROOT_INIT_DIR%/3rd/jsoncpp
set TINYXML2_INIT_DIR=%ROOT_INIT_DIR%/3rd/tinyxml2
SET CCRONEXPR_INIT_DIR=%ROOT_INIT_DIR%/3rd/ccronexpr

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
if not exist %SPDLOG_INIT_DIR% (
    git clone -b v1.11.0 https://github.com/gabime/spdlog.git %SPDLOG_INIT_DIR%
) else (
    cd %SPDLOG_INIT_DIR%
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

cd %ROOT_INIT_DIR%
if not exist %TINYXML2_INIT_DIR% (
    git clone https://github.com/leethomason/tinyxml2.git %TINYXML2_INIT_DIR%
) else (
    cd %TINYXML2_INIT_DIR%
    git checkout .
    git pull
)

cd %ROOT_INIT_DIR%
if not exist %CCRONEXPR_INIT_DIR% (
    git clone https://github.com/staticlibs/ccronexpr.git %CCRONEXPR_INIT_DIR%
) else (
    cd %CCRONEXPR_INIT_DIR%
    git checkout .
    git pull
)

pause