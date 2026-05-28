@echo off

set ROOT_DIR=%~dp0
set SERVER_LINK=%ROOT_DIR%\server_d.sln
set SERVER_TARGET=%ROOT_DIR%\.build\windows\x64-Debug\server.sln

@rem server debug shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %SERVER_LINK% (
        mklink "%SERVER_LINK%" "%SERVER_TARGET%"
    )
) else (
    echo Please run as administrator privileges.
)

pause
