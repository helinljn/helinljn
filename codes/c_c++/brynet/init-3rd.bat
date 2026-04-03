@echo off

set ROOT_INIT_DIR=%~dp0
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set SPDLOG_INIT_DIR=%ROOT_INIT_DIR%/3rd/spdlog
set DOCTEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/doctest
set SIMPLEINI_INIT_DIR=%ROOT_INIT_DIR%/3rd/simpleini
set JSONCPP_INIT_DIR=%ROOT_INIT_DIR%/3rd/jsoncpp
set BRYNET_INIT_DIR=%ROOT_INIT_DIR%/3rd/brynet
set LIGHTHOOK_INIT_DIR=%ROOT_INIT_DIR%/3rd/LightHook
set DISTORM_INIT_DIR=%ROOT_INIT_DIR%/3rd/distorm
set FUNCHOOK_INIT_DIR=%ROOT_INIT_DIR%/3rd/funchook

echo ------------------
echo -- fmt
cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 11.2.0 https://github.com/fmtlib/fmt.git %FMT_INIT_DIR%
) else (
    cd %FMT_INIT_DIR%
    git remote set-url origin https://github.com/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 11.2.0
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
echo -- simpleini
cd %ROOT_INIT_DIR%
if not exist %SIMPLEINI_INIT_DIR% (
    git clone -b v4.25 https://github.com/brofield/simpleini.git %SIMPLEINI_INIT_DIR%
) else (
    cd %SIMPLEINI_INIT_DIR%
    git remote set-url origin https://github.com/brofield/simpleini.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v4.25
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
echo -- LightHook
cd %ROOT_INIT_DIR%
if not exist %LIGHTHOOK_INIT_DIR% (
    git clone https://github.com/SamuelTulach/LightHook.git %LIGHTHOOK_INIT_DIR%
) else (
    cd %LIGHTHOOK_INIT_DIR%
    git remote set-url origin https://github.com/SamuelTulach/LightHook.git
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

pause