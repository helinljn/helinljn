@echo off

set ROOT_DIR=%~dp0
set POCO_LINK=%ROOT_DIR%\poco_d.sln
set POCO_TARGET=%ROOT_DIR%\3rd\poco\build\Poco.sln
set SERVER_LINK=%ROOT_DIR%\server_d.sln
set SERVER_TARGET=%ROOT_DIR%\.build\windows\x64-Debug\server.sln

@rem server_d
if not exist %SERVER_LINK% (
    mklink "%SERVER_LINK%" "%SERVER_TARGET%"
)

@rem poco_d
if not exist %POCO_LINK% (
    mklink "%POCO_LINK%" "%POCO_TARGET%"
)

pause