@echo off

for %%f in (*.proto) do (
    echo %%f
    ..\..\..\.build\Release\protoc318.exe %%f --cpp_out=..
)

pause