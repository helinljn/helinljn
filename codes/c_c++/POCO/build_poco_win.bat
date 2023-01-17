@echo off

set POCO_INIT_DIR=%CD%\poco
set POCO_OPENSSL_DIR=%POCO_INIT_DIR%\..\..\3rd\openssl-1.1.1n\x64-windows\include
set POCO_MYSQL_CLIENT_DIR=%POCO_INIT_DIR%\..\..\3rd\mysqlclient-8.0.30\x64-windows\include

if not exist %POCO_INIT_DIR% (
    git clone https://github.com/pocoproject/poco.git %POCO_INIT_DIR%
)

cd %POCO_INIT_DIR%

git checkout poco-1.12.4-release

xcopy %POCO_OPENSSL_DIR%\openssl %POCO_INIT_DIR%\Crypto\include\openssl /e /i /y /q
xcopy %POCO_MYSQL_CLIENT_DIR%\mysql %POCO_INIT_DIR%\Data\MySQL\include\mysql /e /i /y /q

(
    echo CppUnit
    echo Foundation
    echo XML
    echo JSON
    echo Util
    echo Net
    echo Crypto
    echo NetSSL_OpenSSL
    echo Data
    echo Data/MySQL
    echo Zip
    echo JWT
    echo Redis
) > components

cmd /c "buildwin.cmd 160 build static_mt both x64 nosamples notests msbuild noenv"

git checkout -- components

rd /s /q %POCO_INIT_DIR%\Crypto\include\openssl
rd /s /q %POCO_INIT_DIR%\Data\MySQL\include\mysql
del /q %POCO_INIT_DIR%\lib64\Poco*64*

pause