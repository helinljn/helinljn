@echo off
setlocal enabledelayedexpansion

set VS_VERSION="Visual Studio 17 2022"
set ROOT_DIR=%~dp0
set DEBUG_DIR=%ROOT_DIR%/.build/windows/x64-Debug
set RELEASE_DIR=%ROOT_DIR%/.build/windows/x64-Release
set BUILD_PARALLEL=%NUMBER_OF_PROCESSORS%

if "%BUILD_PARALLEL%" equ "" (
    set BUILD_PARALLEL=1
)

if "%1" equ "debug" (
    mkdir "%DEBUG_DIR%" 2>nul

    cd "%DEBUG_DIR%"

    cmake -G %VS_VERSION% -A x64 -C "%ROOT_DIR%poco.cmake" -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug ../../../
    if errorlevel 1 exit /b 1

    cmake --build . --config Debug --parallel %BUILD_PARALLEL%
    if errorlevel 1 exit /b 1

    cd "%ROOT_DIR%"
) else if "%1" equ "release" (
    mkdir "%RELEASE_DIR%" 2>nul

    cd "%RELEASE_DIR%"

    cmake -G %VS_VERSION% -A x64 -C "%ROOT_DIR%poco.cmake" -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_BUILD_TYPE=Release ../../../
    if errorlevel 1 exit /b 1

    cmake --build . --config Release --parallel %BUILD_PARALLEL%
    if errorlevel 1 exit /b 1

    cd "%ROOT_DIR%"
) else if "%1" equ "all" (
    mkdir "%DEBUG_DIR%" 2>nul

    cd "%DEBUG_DIR%"

    cmake -G %VS_VERSION% -A x64 -C "%ROOT_DIR%poco.cmake" -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug ../../../
    if errorlevel 1 exit /b 1

    cmake --build . --config Debug --parallel %BUILD_PARALLEL%
    if errorlevel 1 exit /b 1
    echo.
    echo.
    echo.

    cd "%ROOT_DIR%"

    mkdir "%RELEASE_DIR%" 2>nul

    cd "%RELEASE_DIR%"

    cmake -G %VS_VERSION% -A x64 -C "%ROOT_DIR%poco.cmake" -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_BUILD_TYPE=Release ../../../
    if errorlevel 1 exit /b 1

    cmake --build . --config Release --parallel %BUILD_PARALLEL%
    if errorlevel 1 exit /b 1

    cd "%ROOT_DIR%"
) else (
    echo "Usage: build.poco.bat [debug | release | all]"
)

exit /b 0
