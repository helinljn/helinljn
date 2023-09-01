@echo off

set ROOT_DIR=%~dp0

@rem server_d
set SERVER_D=%ROOT_DIR%\server_d.sln
set TARGET_D=%ROOT_DIR%\.build\windows\x64-Debug\server.sln

@rem server_r
set SERVER_R=%ROOT_DIR%\server_r.sln
set TARGET_R=%ROOT_DIR%\.build\windows\x64-Release\server.sln

@rem poco_vs160
set SERVER_POCO=%ROOT_DIR%\poco_vs160.sln
set TARGET_POCO=%ROOT_DIR%\tools\poco_vs160\poco_vs160.sln

@rem server_d
if exist %SERVER_D% (
    del /f /q "%SERVER_D%"
)

if exist %TARGET_D% (
    mklink "%SERVER_D%" "%TARGET_D%"
)

@rem server_r
if exist %SERVER_R% (
    del /f /q "%SERVER_R%"
)

if exist %TARGET_R% (
    mklink "%SERVER_R%" "%TARGET_R%"
)

@rem poco_vs160
if exist %SERVER_POCO% (
    del /f /q "%SERVER_POCO%"
)

if exist %TARGET_POCO% (
    mklink "%SERVER_POCO%" "%TARGET_POCO%"
)

pause