mkdir -p build/linux/x64-Release

cd build/linux/x64-Release

cmake -DCMAKE_BUILD_TYPE=Release ../../../

cmake --build .
