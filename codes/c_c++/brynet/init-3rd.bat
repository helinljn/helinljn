@echo off

set ROOT_INIT_DIR=%~dp0
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set QUILL_INIT_DIR=%ROOT_INIT_DIR%/3rd/quill
set BRYNET_INIT_DIR=%ROOT_INIT_DIR%/3rd/brynet
set GOOGLETEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/googletest

echo ------------------
echo -- fmt
cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 10.2.1 https://kkgithub.com/fmtlib/fmt.git %FMT_INIT_DIR%
) else (
    cd %FMT_INIT_DIR%
    git remote set-url origin https://kkgithub.com/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 10.2.1
)

echo ------------------
echo -- quill
cd %ROOT_INIT_DIR%
if not exist %QUILL_INIT_DIR% (
    git clone -b v3.8.0 https://kkgithub.com/odygrd/quill.git %QUILL_INIT_DIR%
) else (
    cd %QUILL_INIT_DIR%
    git remote set-url origin https://kkgithub.com/odygrd/quill.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout v3.8.0
)

echo ------------------
echo -- brynet
cd %ROOT_INIT_DIR%
if not exist %BRYNET_INIT_DIR% (
    git clone https://kkgithub.com/IronsDu/brynet.git %BRYNET_INIT_DIR%
) else (
    cd %BRYNET_INIT_DIR%
    git remote set-url origin https://kkgithub.com/IronsDu/brynet.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- googletest
cd %ROOT_INIT_DIR%
if not exist %GOOGLETEST_INIT_DIR% (
    git clone -b v1.14.x https://kkgithub.com/google/googletest.git %GOOGLETEST_INIT_DIR%
) else (
    cd %GOOGLETEST_INIT_DIR%
    git remote set-url origin https://kkgithub.com/google/googletest.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

pause