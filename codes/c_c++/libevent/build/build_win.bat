@echo off

mkdir build\windows

cd build\windows

cmake -DCMAKE_BUILD_TYPE=Release ..\..\

cmake --build .

cd ..\..\lib

move /y Debug\event_all_r.lib .

del /q Debug\*
rd  /q Debug

pause