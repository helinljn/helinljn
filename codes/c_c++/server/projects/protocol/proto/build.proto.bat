@echo off

for %%f in (*.proto) do (
    echo %%f
    ..\..\..\.build\Release\protoc-exec.exe %%f --cpp_out=..
)

pause