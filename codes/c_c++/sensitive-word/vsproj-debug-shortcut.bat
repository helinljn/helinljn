@echo off

set ROOT_DIR=%~dp0
set SENSITIVE_WORLD_LINK=%ROOT_DIR%\sensitive-word_d.sln
set SENSITIVE_WORLD_TARGET=%ROOT_DIR%\.build\windows\x64-Debug\sensitive-word.sln

@rem sensitive-word debug shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %SENSITIVE_WORLD_LINK% (
        mklink "%SENSITIVE_WORLD_LINK%" "%SENSITIVE_WORLD_TARGET%"
    )
) else (
    echo Please run as administrator privileges.
)

pause