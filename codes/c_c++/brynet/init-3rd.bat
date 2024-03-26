@echo off

set ROOT_INIT_DIR=%~dp0
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set QUILL_INIT_DIR=%ROOT_INIT_DIR%/3rd/quill
set BRYNET_INIT_DIR=%ROOT_INIT_DIR%/3rd/brynet

echo ------------------
echo -- fmt
cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 10.2.1 https://github.hscsec.cn/fmtlib/fmt.git %FMT_INIT_DIR%
) else (
    cd %FMT_INIT_DIR%
    git remote set-url origin https://github.hscsec.cn/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 10.2.1
)

echo ------------------
echo -- quill
cd %ROOT_INIT_DIR%
if not exist %QUILL_INIT_DIR% (
    git clone -b v3.8.0 https://github.hscsec.cn/odygrd/quill.git %QUILL_INIT_DIR%
) else (
    cd %QUILL_INIT_DIR%
    git remote set-url origin https://github.hscsec.cn/odygrd/quill.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v3.8.0
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