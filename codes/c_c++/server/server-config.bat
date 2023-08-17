@echo off

set VS_VERSION="Visual Studio 16 2019"
set ROOT_DIR=%~dp0
set DEBUG_DIR=%ROOT_DIR%/.build/windows/x64-Debug
set RELEASE_DIR=%ROOT_DIR%/.build/windows/x64-Release

@rem Debug
cd %ROOT_DIR%
mkdir "%DEBUG_DIR%" 2>nul
cd %DEBUG_DIR%

cmake -G %VS_VERSION% -A x64 -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug ../../../
echo.
echo.
echo.

@rem Release
cd %ROOT_DIR%
mkdir "%RELEASE_DIR%" 2>nul
cd %RELEASE_DIR%

cmake -G %VS_VERSION% -A x64 -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_BUILD_TYPE=Release ../../../
echo.
echo.
echo.

pause