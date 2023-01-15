@echo off

set POCO_INIT_DIR=%CD%\poco
set POCO_OPENSSL_DIR=%POCO_INIT_DIR%\..\..\vcpkg\vcpkg\packages\openssl_x64-windows-static\include
set POCO_BRANCH_NAME=poco-1.12.4-release

if not exist %POCO_INIT_DIR% (
    git clone https://github.com/pocoproject/poco.git %POCO_INIT_DIR%
)

cd %POCO_INIT_DIR%

git checkout %POCO_BRANCH_NAME%

xcopy %POCO_OPENSSL_DIR%\openssl %POCO_INIT_DIR%\Crypto\include\openssl /e /i /y /q

(
    echo Foundation
    echo XML
    echo JSON
    echo Util
    echo Net
    echo Crypto
    echo NetSSL_OpenSSL
    echo JWT
    echo Redis
) > components

cmd /c "buildwin.cmd 160 build static_mt both x64 nosamples notests msbuild noenv"

git checkout -- components

rd /s /q %POCO_INIT_DIR%\Crypto\include\openssl

pause