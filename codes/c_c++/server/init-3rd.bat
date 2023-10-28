@echo off

set ROOT_INIT_DIR=%~dp0
set FMT_INIT_DIR=%ROOT_INIT_DIR%/3rd/fmt
set POCO_INIT_DIR=%ROOT_INIT_DIR%/3rd/poco
set HOTPATCH_INIT_DIR=%ROOT_INIT_DIR%/3rd/hotpatch
set PROTOBUF_INIT_DIR=%ROOT_INIT_DIR%/3rd/protobuf
set GOOGLETEST_INIT_DIR=%ROOT_INIT_DIR%/3rd/googletest

echo ------------------
echo -- fmt
cd %ROOT_INIT_DIR%
if not exist %FMT_INIT_DIR% (
    git clone -b 9.1.0 https://hub.nuaa.cf/fmtlib/fmt.git %FMT_INIT_DIR%
) else (
    cd %FMT_INIT_DIR%
    git remote set-url origin https://hub.nuaa.cf/fmtlib/fmt.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout 9.1.0
)

echo ------------------
echo -- poco
cd %ROOT_INIT_DIR%
if not exist %POCO_INIT_DIR% (
    git clone -b poco-1.12.5 https://hub.nuaa.cf/pocoproject/poco.git %POCO_INIT_DIR%
) else (
    cd %POCO_INIT_DIR%
    git remote set-url origin https://hub.nuaa.cf/pocoproject/poco.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- hotpatch
cd %ROOT_INIT_DIR%
if not exist %HOTPATCH_INIT_DIR% (
    git clone -b master https://hub.nuaa.cf/vikasnkumar/hotpatch.git %HOTPATCH_INIT_DIR%
) else (
    cd %HOTPATCH_INIT_DIR%
    git remote set-url origin https://hub.nuaa.cf/vikasnkumar/hotpatch.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- protobuf
cd %ROOT_INIT_DIR%
if not exist %PROTOBUF_INIT_DIR% (
    git clone -b 3.19.x https://hub.nuaa.cf/protocolbuffers/protobuf.git %PROTOBUF_INIT_DIR%
) else (
    cd %PROTOBUF_INIT_DIR%
    git remote set-url origin https://hub.nuaa.cf/protocolbuffers/protobuf.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

echo ------------------
echo -- googletest
cd %ROOT_INIT_DIR%
if not exist %GOOGLETEST_INIT_DIR% (
    git clone -b v1.12.x https://hub.nuaa.cf/google/googletest.git %GOOGLETEST_INIT_DIR%
) else (
    cd %GOOGLETEST_INIT_DIR%
    git remote set-url origin https://hub.nuaa.cf/google/googletest.git
    git remote -v
    git checkout .
    git fetch -p origin
    git pull
)

pause