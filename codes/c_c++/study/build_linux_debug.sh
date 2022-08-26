mkdir -p build/linux/x64-Debug

cd build/linux/x64-Debug

cmake -DCMAKE_BUILD_TYPE=Debug ../../../

cmake --build .
