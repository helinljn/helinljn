@echo off
setlocal enabledelayedexpansion

set VS_VERSION="Visual Studio 17 2022"
set ROOT_DIR=%~dp0
set DEBUG_DIR=%ROOT_DIR%/poco/.build/windows/x64-Debug
set RELEASE_DIR=%ROOT_DIR%/poco/.build/windows/x64-Release
set POCO_MODULES=Foundation Encodings XML JSON Util Net NetSSL Crypto Data DataSQLite DataMySQL Redis JWT Prometheus Zip

goto :main

:build_poco_config
set "POCO_CONFIG=%~1"
set "POCO_BUILD_DIR=%~2"

mkdir "%POCO_BUILD_DIR%" 2>nul
pushd "%POCO_BUILD_DIR%" || exit /b 1

cmake -G %VS_VERSION% -A x64 -C "%ROOT_DIR%poco.cmake" -DCMAKE_CONFIGURATION_TYPES=%POCO_CONFIG% -DCMAKE_BUILD_TYPE=%POCO_CONFIG% ../../../
if errorlevel 1 (
    popd
    exit /b 1
)

cmake --build . --config %POCO_CONFIG%
if errorlevel 1 (
    popd
    exit /b 1
)

popd

call :install_poco_config "%POCO_CONFIG%" "%POCO_BUILD_DIR%"
exit /b %ERRORLEVEL%

:install_poco_config
set "POCO_CONFIG=%~1"
set "POCO_BUILD_DIR=%~2"
set "POCO_BIN_DIR=%POCO_BUILD_DIR%\bin"
set "POCO_LIB_DIR=%POCO_BUILD_DIR%\lib"

if /I "%POCO_CONFIG%"=="Debug" (
    set "POCO_SUFFIX=d"
) else (
    set "POCO_SUFFIX="
)

mkdir "%POCO_LIB_DIR%" 2>nul

for %%M in (%POCO_MODULES%) do (
    call :copy_required "%POCO_BIN_DIR%\Poco%%M%POCO_SUFFIX%.dll" "%POCO_LIB_DIR%"
    if errorlevel 1 exit /b 1

    call :copy_required "%POCO_BIN_DIR%\Poco%%M%POCO_SUFFIX%.lib" "%POCO_LIB_DIR%"
    if errorlevel 1 exit /b 1

    if /I "%POCO_CONFIG%"=="Debug" (
        call :copy_required "%POCO_BIN_DIR%\Poco%%M%POCO_SUFFIX%.pdb" "%POCO_LIB_DIR%"
        if errorlevel 1 exit /b 1
    )
)

exit /b 0

:copy_required
if not exist "%~1" (
    echo Missing Poco build artifact: %~1
    exit /b 1
)

copy /Y "%~1" "%~2\" >nul
if errorlevel 1 exit /b 1

exit /b 0

:main
if /I "%~1"=="debug" (
    call :build_poco_config "Debug" "%DEBUG_DIR%"
    if errorlevel 1 exit /b 1
) else if /I "%~1"=="release" (
    call :build_poco_config "Release" "%RELEASE_DIR%"
    if errorlevel 1 exit /b 1
) else if /I "%~1"=="all" (
    call :build_poco_config "Debug" "%DEBUG_DIR%"
    if errorlevel 1 exit /b 1
    echo.
    echo.
    echo.

    call :build_poco_config "Release" "%RELEASE_DIR%"
    if errorlevel 1 exit /b 1
) else (
    echo "Usage: build.poco.bat [debug | release | all]"
)

exit /b 0
