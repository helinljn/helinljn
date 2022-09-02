@echo off

..\..\..\..\vcpkg\vcpkg\packages\protobuf_x64-windows-static\tools\protobuf\protoc.exe TestMsgDefine.proto --cpp_out=.
..\..\..\..\vcpkg\vcpkg\packages\protobuf_x64-windows-static\tools\protobuf\protoc.exe TestMsgStruct.proto --cpp_out=.

pause