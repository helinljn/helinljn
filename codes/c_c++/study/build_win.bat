@echo off

set vs_version="Visual Studio 16 2019"

if %1 equ debug (
    mkdir .build\windows\x64-Debug
    cd .build\windows\x64-Debug

    cmake -G %vs_version% -A x64 -DCMAKE_INSTALL_PREFIX=. -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build . --config Debug

    cd ../../../
) else if %1 equ release (
    mkdir .build\windows\x64-Release
    cd .build\windows\x64-Release

    cmake -G %vs_version% -A x64 -DCMAKE_INSTALL_PREFIX=. -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build . --config Release

    cd ../../../
) else if %1 equ all (
    mkdir .build\windows\x64-Debug
    cd .build\windows\x64-Debug

    cmake -G %vs_version% -A x64 -DCMAKE_INSTALL_PREFIX=. -DCMAKE_CONFIGURATION_TYPES=Debug -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build . --config Debug

    cd ../../../

    mkdir .build\windows\x64-Release
    cd .build\windows\x64-Release

    cmake -G %vs_version% -A x64 -DCMAKE_INSTALL_PREFIX=. -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build . --config Release

    cd ../../../
) else (
    echo "Usage:build_win.bat [debug | release | all]"
)