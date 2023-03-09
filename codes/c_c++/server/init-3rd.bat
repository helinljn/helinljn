@echo off

set ROOT_INIT_DIR=%CD%
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set ASIO_INIT_DIR=%ROOT_INIT_DIR%/3rd/asio
set SPDLOG_INIT_DIR=%ROOT_INIT_DIR%/3rd/spdlog
set TINYXML2_INIT_DIR=%ROOT_INIT_DIR%/3rd/tinyxml2
set HIREDIS_INIT_DIR=%ROOT_INIT_DIR%/3rd/hiredis
set JSONCPP_INIT_DIR=%ROOT_INIT_DIR%/3rd/jsoncpp
set CPP_COMMON_INIT_DIR=%ROOT_INIT_DIR%/3rd/CppCommon
set CPP_SERVER_INIT_DIR=%ROOT_INIT_DIR%/3rd/CppServer

cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git %FMT_INIT_DIR%

    cd %FMT_INIT_DIR%
    git log -1
)

cd %ROOT_INIT_DIR%
if not exist %ASIO_INIT_DIR% (
    git clone -b asio-1-22-2 https://github.com/chriskohlhoff/asio.git %ASIO_INIT_DIR%

    cd %ASIO_INIT_DIR%
    git log -1
)

cd %ROOT_INIT_DIR%
if not exist %SPDLOG_INIT_DIR% (
    git clone -b v1.11.0 https://github.com/gabime/spdlog.git %SPDLOG_INIT_DIR%

    cd %SPDLOG_INIT_DIR%
    git log -1
)

cd %ROOT_INIT_DIR%
if not exist %TINYXML2_INIT_DIR% (
    git clone -b 9.0.0 https://github.com/leethomason/tinyxml2.git %TINYXML2_INIT_DIR%

    cd %TINYXML2_INIT_DIR%
    git log -1
)

cd %ROOT_INIT_DIR%
if not exist %HIREDIS_INIT_DIR% (
    git clone -b v1.0.2 https://github.com/redis/hiredis.git %HIREDIS_INIT_DIR%

    cd %HIREDIS_INIT_DIR%
    git log -1
)

cd %ROOT_INIT_DIR%
if not exist %JSONCPP_INIT_DIR% (
    git clone https://github.com/open-source-parsers/jsoncpp.git %JSONCPP_INIT_DIR%
) else (
    cd %JSONCPP_INIT_DIR%
    git pull
    git log -1
)

cd %ROOT_INIT_DIR%
if not exist %CPP_COMMON_INIT_DIR% (
    git clone https://github.com/chronoxor/CppCommon.git %CPP_COMMON_INIT_DIR%
) else (
    cd %CPP_COMMON_INIT_DIR%
    git pull
    git log -1
)

cd %ROOT_INIT_DIR%
if not exist %CPP_SERVER_INIT_DIR% (
    git clone https://github.com/chronoxor/CppServer.git %CPP_SERVER_INIT_DIR%
) else (
    cd %CPP_SERVER_INIT_DIR%
    git pull
    git log -1
)

pause