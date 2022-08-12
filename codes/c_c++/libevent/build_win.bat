@echo off

mkdir build\windows

cd build\windows

cmake -DCMAKE_BUILD_TYPE=Release ..\..\

cmake --build .

cd ..\..\output

move /y Debug\* .

del /q Debug\*
rd  /q Debug

pause