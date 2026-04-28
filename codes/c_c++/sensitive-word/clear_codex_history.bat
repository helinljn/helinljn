@echo off
setlocal

set "TARGET_DIR=%USERPROFILE%\.codex\sessions"

if not exist "%TARGET_DIR%" (
    echo Target folder does not exist:
    echo %TARGET_DIR%
    exit /b 1
)

del /f /q "%TARGET_DIR%\*" >nul 2>nul
for /d %%D in ("%TARGET_DIR%\*") do rd /s /q "%%D"

echo Done.
exit /b 0