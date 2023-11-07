@echo off

set PBMSG_DIR=../../src/common/pbmsg

mkdir "%PBMSG_DIR%" 2>nul

for %%f in (%PBMSG_DIR%/*.proto) do (protoc-exec.exe --proto_path=%PBMSG_DIR% --cpp_out=dllexport_decl=PROTOC_EXPORT:%PBMSG_DIR% %%f)
for %%f in (%PBMSG_DIR%/*.pb.h)  do (..\unix2dos\unix2dos.exe %PBMSG_DIR%/%%f)
for %%f in (%PBMSG_DIR%/*.pb.cc) do (..\unix2dos\unix2dos.exe %PBMSG_DIR%/%%f)

pause