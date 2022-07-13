@echo off

set db_host=localhost
set db_port=3306
set db_user=root
set db_password=123456

call :func_update_db test ./update_test_db.sql

goto :end

:func_update_db
	mysql -h%db_host% -P%db_port% -u%db_user% -p%db_password% --default-character-set=utf8 -D%1 < %2
	echo Update '%1' end!
goto :eof

:end

pause