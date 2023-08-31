@echo off

mkdir out 2>nul

for %%f in (*.proto) do (
    echo %%f
    protoc-exec.exe %%f --cpp_out=./out
)

wsl bash -c "find ./out -name '*.pb.h' | xargs unix2dos"
wsl bash -c "find ./out -name '*.pb.cc' | xargs unix2dos"

pause