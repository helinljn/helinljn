@echo off

for %%f in (*.proto) do (
    echo %%f
    ..\..\..\build\Release\protoc315.exe %%f --cpp_out=..
)

pause