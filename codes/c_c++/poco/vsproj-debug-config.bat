@echo off

set VS_VERSION="Visual Studio 17 2022"
set ROOT_DIR=%~dp0
set POCO_DEBUG_DIR=%ROOT_DIR%/.build/windows/x64-Debug

@rem poco debug config
mkdir "%POCO_DEBUG_DIR%" 2>nul
cd %POCO_DEBUG_DIR%

cmake -G %VS_VERSION% -A x64 -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug ../../../

pause
