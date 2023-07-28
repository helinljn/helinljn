@echo off

set ABS_PATH=D:\helinljn\codes\c_c++\server
set SERVER_D=%ABS_PATH%\server_d.sln
set SERVER_R=%ABS_PATH%\server_r.sln
set TARGET_D=%ABS_PATH%\.build\windows\x64-Debug\server.sln
set TARGET_R=%ABS_PATH%\.build\windows\x64-Release\server.sln

if exist %SERVER_D% (
    del /f /q "%SERVER_D%"
)

if exist %SERVER_R% (
    del /f /q %SERVER_R%
)

if exist %TARGET_D% (
    mklink "%SERVER_D%" "%TARGET_D%"
)

if exist %TARGET_R% (
    mklink "%SERVER_R%" "%TARGET_R%"
)

pause