mkdir -p out/build/linux

cd out/build/linux

cmake -DCMAKE_BUILD_TYPE=release ../../../

cmake --build .
