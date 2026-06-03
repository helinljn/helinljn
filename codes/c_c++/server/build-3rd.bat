@echo off

set ROOT_DIR=%~dp0
set BUILD_DIR=%ROOT_DIR%/3rd/3rd-cmake

if %1 equ debug (
    cd %BUILD_DIR%

    echo.
    echo --------------------------------
    echo -- Building poco...
    call poco.build.bat debug

    echo.
    echo --------------------------------
    echo -- Building curl...
    call curl.build.bat debug

    echo.
    echo --------------------------------
    echo -- Building libhv...
    call libhv.build.bat debug

    cd %ROOT_DIR%
) else if %1 equ release (
    cd %BUILD_DIR%

    echo.
    echo --------------------------------
    echo -- Building poco...
    call poco.build.bat release

    echo.
    echo --------------------------------
    echo -- Building curl...
    call curl.build.bat release

    echo.
    echo --------------------------------
    echo -- Building libhv...
    call libhv.build.bat release

    cd %ROOT_DIR%
) else if %1 equ all (
    cd %BUILD_DIR%

    echo.
    echo --------------------------------
    echo -- Building poco...
    call poco.build.bat all

    echo.
    echo --------------------------------
    echo -- Building curl...
    call curl.build.bat all

    echo.
    echo --------------------------------
    echo -- Building libhv...
    call libhv.build.bat all

    cd %ROOT_DIR%
) else (
    echo Usage: build-3rd.bat [debug ^| release ^| all]
)
