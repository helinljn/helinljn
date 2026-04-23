@echo off

set ROOT_INIT_DIR=%~dp0
set BRYNET_INIT_DIR=%ROOT_INIT_DIR%/3rd/brynet
set UTFCPP_INIT_DIR=%ROOT_INIT_DIR%/3rd/utfcpp
set OPENCC_INIT_DIR=%ROOT_INIT_DIR%/3rd/opencc
set SPDLOG_INIT_DIR=%ROOT_INIT_DIR%/3rd/spdlog
set DOCTEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/doctest
set MIMALLOC_INIT_DIR=%ROOT_INIT_DIR%/3rd/mimalloc

echo ------------------
echo -- brynet
cd %ROOT_INIT_DIR%
if not exist %BRYNET_INIT_DIR% (
    git clone https://github.com/IronsDu/brynet.git %BRYNET_INIT_DIR%
) else (
    cd %BRYNET_INIT_DIR%
    git remote set-url origin https://github.com/IronsDu/brynet.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- utfcpp
cd %ROOT_INIT_DIR%
if not exist %UTFCPP_INIT_DIR% (
    git clone -b v4.0.9 https://github.com/nemtrif/utfcpp.git %UTFCPP_INIT_DIR%
) else (
    cd %UTFCPP_INIT_DIR%
    git remote set-url origin https://github.com/nemtrif/utfcpp.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v4.0.9
)

echo ------------------
echo -- opencc
cd %ROOT_INIT_DIR%
if not exist %OPENCC_INIT_DIR% (
    git clone -b ver.1.2.0 https://github.com/BYVoid/OpenCC.git %OPENCC_INIT_DIR%
) else (
    cd %OPENCC_INIT_DIR%
    git remote set-url origin https://github.com/BYVoid/OpenCC.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout ver.1.2.0
)

echo ------------------
echo -- spdlog
cd %ROOT_INIT_DIR%
if not exist %SPDLOG_INIT_DIR% (
    git clone -b v1.15.3 https://github.com/gabime/spdlog.git %SPDLOG_INIT_DIR%
) else (
    cd %SPDLOG_INIT_DIR%
    git remote set-url origin https://github.com/gabime/spdlog.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v1.15.3
)

echo ------------------
echo -- doctest
cd %ROOT_INIT_DIR%
if not exist %DOCTEST_INIT_DIR% (
    git clone -b v2.4.12 https://github.com/doctest/doctest.git %DOCTEST_INIT_DIR%
) else (
    cd %DOCTEST_INIT_DIR%
    git remote set-url origin https://github.com/doctest/doctest.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v2.4.12
)

echo ------------------
echo -- mimalloc
cd %ROOT_INIT_DIR%
if not exist %MIMALLOC_INIT_DIR% (
    git clone -b v2.3.1 https://github.com/microsoft/mimalloc.git %MIMALLOC_INIT_DIR%
) else (
    cd %MIMALLOC_INIT_DIR%
    git remote set-url origin https://github.com/microsoft/mimalloc.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v2.3.1
)

pause