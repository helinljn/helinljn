mkdir -p build/linux

cd build/linux

cmake -DCMAKE_BUILD_TYPE=release ../../

cmake --build .
