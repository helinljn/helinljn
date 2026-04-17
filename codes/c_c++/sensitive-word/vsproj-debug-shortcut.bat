@echo off

set ROOT_DIR=%~dp0
set SENSITIVE_WORLD_LINK=%ROOT_DIR%\sensitive-word_d.sln
set SENSITIVE_WORLD_TARGET=%ROOT_DIR%\.build\windows\x64-Debug\sensitive-word.sln

@rem sensitive-word debug shortcut config
if not exist %SENSITIVE_WORLD_LINK% (
    mklink "%SENSITIVE_WORLD_LINK%" "%SENSITIVE_WORLD_TARGET%"
)

pause