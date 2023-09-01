@echo off

mkdir pbmsg 2>nul

for %%f in (*.proto) do (
    echo %%f
    protoc-exec.exe %%f --cpp_out=./pbmsg
)

echo.

wsl bash -c "find ./pbmsg -name '*.pb.h' | xargs unix2dos"
wsl bash -c "find ./pbmsg -name '*.pb.cc' | xargs unix2dos"

pause