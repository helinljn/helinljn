@echo off

if %1 equ debug (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

    mkdir .build\windows\x64-Debug
    cd .build\windows\x64-Debug

    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build .

    cd ../../../
) else if %1 equ release (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

    mkdir .build\windows\x64-Release
    cd .build\windows\x64-Release

    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build .

    cd ../../../
) else if %1 equ all (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

    mkdir .build\windows\x64-Debug
    cd .build\windows\x64-Debug

    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../../../
    cmake --build .

    cd ../../../

    mkdir .build\windows\x64-Release
    cd .build\windows\x64-Release

    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ../../../
    cmake --build .

    cd ../../../
) else (
    echo "Usage:build_win.bat [debug | release | all]"
)