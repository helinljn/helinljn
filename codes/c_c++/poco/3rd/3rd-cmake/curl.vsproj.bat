@echo off

set ROOT_DIR=%~dp0
set CURL_LINK=%ROOT_DIR%/curl_d.sln
set CURL_TARGET=%ROOT_DIR%/../curl/.build/windows/x64-Debug/curl.sln

@rem curl debug shortcut
whoami /priv | findstr /i "SeSecurityPrivilege" >nul 2>&1
if %errorlevel% equ 0 (
    if not exist %CURL_LINK% (
        mklink "%CURL_LINK%" "%CURL_TARGET%"
    )
) else (
    echo Please run as administrator privileges.
)

pause
