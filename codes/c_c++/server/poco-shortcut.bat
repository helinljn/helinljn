@echo off

set ROOT_DIR=%~dp0
set POCO_LINK=%ROOT_DIR%\poco_d.sln
set POCO_TARGET=%ROOT_DIR%\3rd\poco\build\Poco.sln

@rem poco_d
if not exist %POCO_LINK% (
    mklink "%POCO_LINK%" "%POCO_TARGET%"
)

pause