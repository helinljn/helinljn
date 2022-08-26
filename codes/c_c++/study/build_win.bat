@echo off

mkdir build\windows

cd build\windows

cmake -DCMAKE_BUILD_TYPE=release ..\..\

cmake --build .

cd ..\..\output\release

move /y Debug\* .

del /q Debug\*
rd  /q Debug

pause