@echo off

set CUR_DIR=D:\helinljn\codes\c_c++\server
set SERVER_D=%CUR_DIR%\server_d.sln
set SERVER_R=%CUR_DIR%\server_r.sln

IF EXIST %SERVER_D% (
    del /f /q "%SERVER_D%"
)

IF EXIST %SERVER_R% (
    del /f /q %SERVER_R%
)

mklink "%SERVER_D%" "%CUR_DIR%\.build\windows\x64-Debug\server.sln"
mklink "%SERVER_R%" "%CUR_DIR%\.build\windows\x64-Release\server.sln"

pause