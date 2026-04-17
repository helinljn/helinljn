@echo off

set ROOT_DIR=%~dp0
set BRYNET_LINK=%ROOT_DIR%\brynet_d.sln
set BRYNET_TARGET=%ROOT_DIR%\.build\windows\x64-Debug\brynet.sln

@rem brynet debug shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %BRYNET_LINK% (
        mklink "%BRYNET_LINK%" "%BRYNET_TARGET%"
    )
) else (
    echo Please run as administrator privileges.
)

pause