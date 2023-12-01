@echo off

set ROOT_INIT_DIR=%~dp0
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set POCO_INIT_DIR=%ROOT_INIT_DIR%/3rd/poco
set PROTOBUF_INIT_DIR=%ROOT_INIT_DIR%/3rd/protobuf
set GOOGLETEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/googletest
set PLTHOOK_INIT_DIR=%ROOT_INIT_DIR%/3rd/plthook
set DISTORM_INIT_DIR=%ROOT_INIT_DIR%/3rd/distorm
set FUNCHOOK_INIT_DIR=%ROOT_INIT_DIR%/3rd/funchook

echo ------------------
echo -- fmt
cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 9.1.0 https://hub.njuu.cf/fmtlib/fmt.git %FMT_INIT_DIR%
) else (
    cd %FMT_INIT_DIR%
    git remote set-url origin https://hub.njuu.cf/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 9.1.0
)

echo ------------------
echo -- poco
cd %ROOT_INIT_DIR%
if not exist %POCO_INIT_DIR% (
    git clone -b poco-1.12.5 https://hub.njuu.cf/pocoproject/poco.git %POCO_INIT_DIR%
) else (
    cd %POCO_INIT_DIR%
    git remote set-url origin https://hub.njuu.cf/pocoproject/poco.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- protobuf
cd %ROOT_INIT_DIR%
if not exist %PROTOBUF_INIT_DIR% (
    git clone -b 3.19.x https://hub.njuu.cf/protocolbuffers/protobuf.git %PROTOBUF_INIT_DIR%
) else (
    cd %PROTOBUF_INIT_DIR%
    git remote set-url origin https://hub.njuu.cf/protocolbuffers/protobuf.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- googletest
cd %ROOT_INIT_DIR%
if not exist %GOOGLETEST_INIT_DIR% (
    git clone -b v1.12.x https://hub.njuu.cf/google/googletest.git %GOOGLETEST_INIT_DIR%
) else (
    cd %GOOGLETEST_INIT_DIR%
    git remote set-url origin https://hub.njuu.cf/google/googletest.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- plthook
cd %ROOT_INIT_DIR%
if not exist %PLTHOOK_INIT_DIR% (
    git clone https://hub.njuu.cf/kubo/plthook.git %PLTHOOK_INIT_DIR%
) else (
    cd %PLTHOOK_INIT_DIR%
    git remote set-url origin https://hub.njuu.cf/kubo/plthook.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- distorm
cd %ROOT_INIT_DIR%
if not exist %DISTORM_INIT_DIR% (
    git clone https://hub.njuu.cf/gdabah/distorm.git %DISTORM_INIT_DIR%
) else (
    cd %DISTORM_INIT_DIR%
    git remote set-url origin https://hub.njuu.cf/gdabah/distorm.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- funchook
cd %ROOT_INIT_DIR%
if not exist %FUNCHOOK_INIT_DIR% (
    git clone https://hub.njuu.cf/kubo/funchook.git %FUNCHOOK_INIT_DIR%
) else (
    cd %FUNCHOOK_INIT_DIR%
    git remote set-url origin https://hub.njuu.cf/kubo/funchook.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

pause