@echo off

set VS_VERSION="Visual Studio 16 2019"
set ROOT_DIR=%~dp0
set SERVER_DEBUG_DIR=%ROOT_DIR%/.build/windows/x64-Debug

@rem server debug config
cd %ROOT_DIR%
mkdir "%SERVER_DEBUG_DIR%" 2>nul
cd %SERVER_DEBUG_DIR%

cmake -G %VS_VERSION% -A x64 -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug ../../../

pause