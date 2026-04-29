@echo off
setlocal

@rem npm i -g @anthropic-ai/claude-code@latest
@rem npm i -g @google/gemini-cli@latest
@rem npm i -g @openai/codex@latest

@rem gemini --list-sessions
@rem gemini --delete-session [id]

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

echo done
pause