@echo off

set ROOT_INIT_DIR=%CD%
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set ASIO_INIT_DIR=%ROOT_INIT_DIR%/3rd/asio
set COMMON_INIT_DIR=%ROOT_INIT_DIR%/3rd/CppCommon
set SERVER_INIT_DIR=%ROOT_INIT_DIR%/3rd/CppServer

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
if not exist %COMMON_INIT_DIR% (
    git clone https://github.com/chronoxor/CppCommon.git %COMMON_INIT_DIR%
) else (
    cd %COMMON_INIT_DIR%
    git pull
    git log -1
)

cd %ROOT_INIT_DIR%
if not exist %SERVER_INIT_DIR% (
    git clone https://github.com/chronoxor/CppServer.git %SERVER_INIT_DIR%
) else (
    cd %SERVER_INIT_DIR%
    git pull
    git log -1
)

pause