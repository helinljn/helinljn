@echo off

set ROOT_DIR=%~dp0
set BRYNET_LINK=%ROOT_DIR%\brynet_d.sln
set BRYNET_TARGET=%ROOT_DIR%\.build\windows\x64-Debug\brynet.sln

@rem brynet_d
if not exist %BRYNET_LINK% (
    mklink "%BRYNET_LINK%" "%BRYNET_TARGET%"
)

pause