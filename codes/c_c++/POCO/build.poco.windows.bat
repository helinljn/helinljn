@echo off

set POCO_INIT_DIR=%CD%\poco
set POCO_OPENSSL_DIR=%POCO_INIT_DIR%\..\..\3rd-libs\openssl-1.1.1n\x64-windows\include

if not exist %POCO_INIT_DIR% (
    git clone -b poco-1.12.4-release https://github.com/pocoproject/poco.git %POCO_INIT_DIR%

    cd %POCO_INIT_DIR%
    git log -1
)

cd %POCO_INIT_DIR%

xcopy %POCO_OPENSSL_DIR%\openssl %POCO_INIT_DIR%\Crypto\include\openssl /e /i /y /q

(
    echo Foundation
    echo XML
    echo JSON
    echo Util
    echo Net
    echo Crypto
    echo NetSSL_OpenSSL
    echo Zip
) > components

cmd /c "buildwin.cmd 160 build static_mt both x64 nosamples notests msbuild noenv"

git checkout -- components

rd /s /q %POCO_INIT_DIR%\Crypto\include\openssl
del /q %POCO_INIT_DIR%\lib64\Poco*64*

pause