@echo off

set ROOT_INIT_DIR=%~dp0
set DISTORM_INIT_DIR=%ROOT_INIT_DIR%/3rd/distorm
set FUNCHOOK_INIT_DIR=%ROOT_INIT_DIR%/3rd/funchook
set POCO_INIT_DIR=%ROOT_INIT_DIR%/3rd/poco
set DOCTEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/doctest
set MIMALLOC_INIT_DIR=%ROOT_INIT_DIR%/3rd/mimalloc

echo ------------------
echo -- distorm
cd %ROOT_INIT_DIR%
if not exist %DISTORM_INIT_DIR% (
    git clone https://github.com/helinljn/distorm.git %DISTORM_INIT_DIR%
) else (
    cd %DISTORM_INIT_DIR%
    git remote set-url origin https://github.com/helinljn/distorm.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- funchook
cd %ROOT_INIT_DIR%
if not exist %FUNCHOOK_INIT_DIR% (
    git clone https://github.com/helinljn/funchook.git %FUNCHOOK_INIT_DIR%
) else (
    cd %FUNCHOOK_INIT_DIR%
    git remote set-url origin https://github.com/helinljn/funchook.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- poco
cd %ROOT_INIT_DIR%
if not exist %POCO_INIT_DIR% (
    git clone -b poco-1.15.3-release https://github.com/pocoproject/poco.git %POCO_INIT_DIR%
) else (
    cd %POCO_INIT_DIR%
    git remote set-url origin https://github.com/pocoproject/poco.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout poco-1.15.3-release
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

pause
