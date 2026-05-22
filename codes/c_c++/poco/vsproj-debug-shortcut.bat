@echo off

set ROOT_DIR=%~dp0
set POCO_LINK=%ROOT_DIR%\poco_d.sln
set POCO_TARGET=%ROOT_DIR%\.build\windows\x64-Debug\poco.sln

@rem poco debug shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %POCO_LINK% (
        mklink "%POCO_LINK%" "%POCO_TARGET%"
    )
) else (
    echo Please run as administrator privileges.
)

pause
