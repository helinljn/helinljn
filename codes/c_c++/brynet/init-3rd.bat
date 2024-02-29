@echo off

set ROOT_INIT_DIR=%~dp0
set SPDLOG_INIT_DIR=%ROOT_INIT_DIR%/3rd/spdlog
set BRYNET_INIT_DIR=%ROOT_INIT_DIR%/3rd/brynet

echo ------------------
echo -- spdlog
cd %ROOT_INIT_DIR%
if not exist %SPDLOG_INIT_DIR% (
    git clone -b v1.13.0 https://github.hscsec.cn/gabime/spdlog.git %SPDLOG_INIT_DIR%
) else (
    cd %SPDLOG_INIT_DIR%
    git remote set-url origin https://github.hscsec.cn/gabime/spdlog.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v1.13.0
)

echo ------------------
echo -- brynet
cd %ROOT_INIT_DIR%
if not exist %BRYNET_INIT_DIR% (
    git clone https://github.hscsec.cn/IronsDu/brynet.git %BRYNET_INIT_DIR%
) else (
    cd %BRYNET_INIT_DIR%
    git remote set-url origin https://github.hscsec.cn/IronsDu/brynet.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

pause