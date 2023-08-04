@echo off

for %%f in (*.proto) do (
    echo %%f
    protoc-exec.exe %%f --cpp_out=..
)

pause