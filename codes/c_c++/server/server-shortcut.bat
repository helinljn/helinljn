@echo off

set ROOT_DIR=%~dp0
set SERVER_D=%ROOT_DIR%\server_d.sln
set SERVER_R=%ROOT_DIR%\server_r.sln
set TARGET_D=%ROOT_DIR%\.build\windows\x64-Debug\server.sln
set TARGET_R=%ROOT_DIR%\.build\windows\x64-Release\server.sln

if exist %SERVER_D% (
    del /f /q "%SERVER_D%"
)

if exist %SERVER_R% (
    del /f /q "%SERVER_R%"
)

if exist %TARGET_D% (
    mklink "%SERVER_D%" "%TARGET_D%"
)

if exist %TARGET_R% (
    mklink "%SERVER_R%" "%TARGET_R%"
)

pause