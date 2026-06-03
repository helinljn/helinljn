@echo off

set ROOT_DIR=%~dp0
set LIBHV_LINK=%ROOT_DIR%/libhv_d.sln
set LIBHV_TARGET=%ROOT_DIR%/../libhv/.build/windows/x64-Debug/libhv.sln

@rem libhv debug shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %LIBHV_LINK% (
        mklink "%LIBHV_LINK%" "%LIBHV_TARGET%"
    )
) else (
    echo Please run as administrator privileges.
)

pause
