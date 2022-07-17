@echo off

mkdir build\windows

cd build\windows

cmake -DCMAKE_BUILD_TYPE=Release ..\..\

cmake --build .

pause