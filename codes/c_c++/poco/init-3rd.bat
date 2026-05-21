@echo off

set ROOT_INIT_DIR=%~dp0
set POCO_INIT_DIR=%ROOT_INIT_DIR%/3rd/poco

echo ------------------
echo -- poco
cd %ROOT_INIT_DIR%
if not exist %POCO_INIT_DIR% (
    git clone -b poco-1.15.3-release https://github.com/pocoproject/poco.git %POCO_INIT_DIR%
) else (
    cd %POCO_INIT_DIR%
    git remote set-url origin https://github.com/pocoproject/poco.git
    git remote -v
    git checkout .
    git fetch -p origin
    git checkout poco-1.15.3-release
)

pause
