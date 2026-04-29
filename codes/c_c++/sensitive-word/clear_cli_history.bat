@echo off
setlocal

rem gemini --list-sessions
rem gemini --delete-session [id]

set "CODEX_DIR=%USERPROFILE%\.codex\sessions"
set "CLAUDE_DIR=%USERPROFILE%\.claude\projects"

if exist "%CODEX_DIR%" (
    del /f /q "%CODEX_DIR%\*" >nul 2>nul
    for /d %%D in ("%CODEX_DIR%\*") do rd /s /q "%%D"
)

if exist "%CLAUDE_DIR%" (
    del /f /q "%CLAUDE_DIR%\*" >nul 2>nul
    for /d %%D in ("%CLAUDE_DIR%\*") do rd /s /q "%%D"
)

echo Done.
exit /b 0