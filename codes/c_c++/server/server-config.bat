@echo off

set OPENSSL_ROOT_DIR=%~dp0/3rd/3rd-libs/openssl-1.1.1t/x64-windows
set MYSQL_DIR=%~dp0/3rd/3rd-libs/mysql-8.0.33/x64-windows
%*

set VS_VERSION="Visual Studio 16 2019"
set ROOT_DIR=%~dp0
set POCO_DEBUG_DIR=%ROOT_DIR%/3rd/poco/build
set SERVER_DEBUG_DIR=%ROOT_DIR%/.build/windows/x64-Debug

echo. > %MYSQL_DIR%\lib\mysqlclient.lib

@rem poco debug config
cd %ROOT_DIR%
mkdir "%POCO_DEBUG_DIR%" 2>nul
cd %POCO_DEBUG_DIR%

cmake -G %VS_VERSION% -A x64 -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=OFF -DENABLE_NETSSL=ON -DENABLE_ENCODINGS=OFF -DENABLE_MONGODB=OFF -DENABLE_DATA_ODBC=OFF -DENABLE_DATA_SQLITE=OFF -DENABLE_PROMETHEUS=OFF -DENABLE_PAGECOMPILER=OFF -DENABLE_PAGECOMPILER_FILE2PAGE=OFF -DENABLE_ACTIVERECORD=OFF -DENABLE_ACTIVERECORD_COMPILER=OFF -DENABLE_TESTS=ON ../
echo.
echo.
echo.

@rem server debug config
cd %ROOT_DIR%
mkdir "%SERVER_DEBUG_DIR%" 2>nul
cd %SERVER_DEBUG_DIR%

cmake -G %VS_VERSION% -A x64 -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug ../../../
echo.
echo.
echo.

del /f /q "%MYSQL_DIR%\lib\mysqlclient.lib"

pause