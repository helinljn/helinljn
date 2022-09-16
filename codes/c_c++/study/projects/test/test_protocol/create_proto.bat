@echo off

..\..\..\build\Release\protoc315.exe TestMsgDefine.proto --cpp_out=.
..\..\..\build\Release\protoc315.exe TestMsgStruct.proto --cpp_out=.

pause