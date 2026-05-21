@echo off

set ROOT_DIR=%~dp0
set POCO_LINK_D=%ROOT_DIR%\..\poco_d.sln
set POCO_TARGET_D=%ROOT_DIR%\poco\.build\windows\x64-Debug\poco.sln
set POCO_LINK_R=%ROOT_DIR%\..\poco_r.sln
set POCO_TARGET_R=%ROOT_DIR%\poco\.build\windows\x64-Release\poco.sln

@rem poco debug shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %POCO_LINK_D% (
        mklink "%POCO_LINK_D%" "%POCO_TARGET_D%"
    )
) else (
    echo Please run as administrator privileges.
)

@rem poco release shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %POCO_LINK_R% (
        mklink "%POCO_LINK_R%" "%POCO_TARGET_R%"
    )
) else (
    echo Please run as administrator privileges.
)

pause
