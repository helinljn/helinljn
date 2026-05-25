@echo off

set ROOT_DIR=%~dp0
set MIMALLOC_LINK=%ROOT_DIR%\mimalloc_d.sln
set MIMALLOC_TARGET=%ROOT_DIR%\mimalloc\.build\windows\x64-Debug\libmimalloc.sln

@rem mimalloc debug shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %MIMALLOC_LINK% (
        mklink "%MIMALLOC_LINK%" "%MIMALLOC_TARGET%"
    )
) else (
    echo Please run as administrator privileges.
)

pause
