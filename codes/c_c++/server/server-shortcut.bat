@echo off

set ROOT_DIR=%~dp0
set SERVER_LINK=%ROOT_DIR%\server_d.sln
set SERVER_TARGET=%ROOT_DIR%\.build\windows\x64-Debug\server.sln

@rem server_d
if not exist %SERVER_LINK% (
    mklink "%SERVER_LINK%" "%SERVER_TARGET%"
)

pause