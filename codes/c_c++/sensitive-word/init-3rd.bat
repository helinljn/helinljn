@echo off

set ROOT_INIT_DIR=%~dp0
set BRYNET_INIT_DIR=%ROOT_INIT_DIR%/3rd/brynet
set DISTORM_INIT_DIR=%ROOT_INIT_DIR%/3rd/distorm
set FUNCHOOK_INIT_DIR=%ROOT_INIT_DIR%/3rd/funchook
set UTFCPP_INIT_DIR=%ROOT_INIT_DIR%/3rd/utfcpp
set OPENCC_INIT_DIR=%ROOT_INIT_DIR%/3rd/opencc
set SPDLOG_INIT_DIR=%ROOT_INIT_DIR%/3rd/spdlog
set JSONCPP_INIT_DIR=%ROOT_INIT_DIR%/3rd/jsoncpp
set DOCTEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/doctest
set MIMALLOC_INIT_DIR=%ROOT_INIT_DIR%/3rd/mimalloc
set NANOBENCH_INIT_DIR=%ROOT_INIT_DIR%/3rd/nanobench

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
echo -- distorm
cd %ROOT_INIT_DIR%
if not exist %DISTORM_INIT_DIR% (
    git clone https://github.com/gdabah/distorm.git %DISTORM_INIT_DIR%
) else (
    cd %DISTORM_INIT_DIR%
    git remote set-url origin https://github.com/gdabah/distorm.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- funchook
cd %ROOT_INIT_DIR%
if not exist %FUNCHOOK_INIT_DIR% (
    git clone https://github.com/kubo/funchook.git %FUNCHOOK_INIT_DIR%
) else (
    cd %FUNCHOOK_INIT_DIR%
    git remote set-url origin https://github.com/kubo/funchook.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- utfcpp
cd %ROOT_INIT_DIR%
if not exist %UTFCPP_INIT_DIR% (
    git clone -b v4.1.0 https://github.com/nemtrif/utfcpp.git %UTFCPP_INIT_DIR%
) else (
    cd %UTFCPP_INIT_DIR%
    git remote set-url origin https://github.com/nemtrif/utfcpp.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v4.1.0
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
echo -- jsoncpp
cd %ROOT_INIT_DIR%
if not exist %JSONCPP_INIT_DIR% (
    git clone -b 1.9.7 https://github.com/open-source-parsers/jsoncpp.git %JSONCPP_INIT_DIR%
) else (
    cd %JSONCPP_INIT_DIR%
    git remote set-url origin https://github.com/open-source-parsers/jsoncpp.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 1.9.7
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
    git clone -b v2.3.2 https://github.com/microsoft/mimalloc.git %MIMALLOC_INIT_DIR%
) else (
    cd %MIMALLOC_INIT_DIR%
    git remote set-url origin https://github.com/microsoft/mimalloc.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v2.3.2
)

echo ------------------
echo -- nanobench
cd %ROOT_INIT_DIR%
if not exist %NANOBENCH_INIT_DIR% (
    git clone -b v4.3.11 https://github.com/martinus/nanobench.git %NANOBENCH_INIT_DIR%
) else (
    cd %NANOBENCH_INIT_DIR%
    git remote set-url origin https://github.com/martinus/nanobench.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v4.3.11
)

pause
