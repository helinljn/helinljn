chcp 65001

@echo Off
setlocal EnableDelayedExpansion

set /p name=Please input project name:

xcopy /S /Y /i template_vs2019 %name%

cd %name%\src\
ren template.cpp %name%.cpp
(
	for /f "tokens=1* delims=:" %%a in ('findstr /n ".*" Makefile') do (
		set str=%%b
		if "!str!" neq "" (
			set str=!str:template=%name%!
		)
		echo,!str!
	)
) > tempMakefile
move /y tempMakefile Makefile
cd ..\..\

cd %name%\proj.linux\
ren template.sh %name%.sh
(
	for /f "tokens=1* delims=:" %%a in ('findstr /n ".*" %name%.sh') do (
		set str=%%b
		if "!str!" neq "" (
			set str=!str:template=%name%!
		)
		echo,!str!
	)
) > temp%name%.sh
move /y temp%name%.sh %name%.sh
cd ..\..\

cd %name%\proj.windows\
ren template.sln %name%.sln
(
	for /f "tokens=1* delims=:" %%a in ('findstr /n ".*" %name%.sln') do (
		set str=%%b
		if "!str!" neq "" (
			set str=!str:template=%name%!
		)
		echo,!str!
	)
) > temp%name%.sln
move /y temp%name%.sln %name%.sln

ren template.vcxproj %name%.vcxproj
(
	for /f "tokens=1* delims=:" %%a in ('findstr /n ".*" %name%.vcxproj') do (
		set str=%%b
		if "!str!" neq "" (
			set str=!str:template=%name%!
		)
		echo,!str!
	)
) > temp%name%.vcxproj
move /y temp%name%.vcxproj %name%.vcxproj

set "rootnamespace=%name%"
:Loop
for /f "tokens=1* delims==-_" %%a in ("%rootnamespace%") do set rootnamespace=%%a%%b
echo,"%rootnamespace%" | findstr "[-_]" > nul && goto :Loop
(
	for /f "tokens=1* delims=:" %%a in ('findstr /n ".*" %name%.vcxproj') do (
		set str=%%b
		if "!str!" neq "" (
			set str=!str:tempspace=%rootnamespace%!
		)
		echo,!str!
	)
) > temp%name%.vcxproj
move /y temp%name%.vcxproj %name%.vcxproj

ren template.vcxproj.filters %name%.vcxproj.filters
(
	for /f "tokens=1* delims=:" %%a in ('findstr /n ".*" %name%.vcxproj.filters') do (
		set str=%%b
		if "!str!" neq "" (
			set str=!str:template=%name%!
		)
		echo,!str!
	)
) > temp%name%.vcxproj.filters
move /y temp%name%.vcxproj.filters %name%.vcxproj.filters

ren template.vcxproj.user %name%.vcxproj.user
(
	for /f "tokens=1* delims=:" %%a in ('findstr /n ".*" %name%.vcxproj.user') do (
		set str=%%b
		if "!str!" neq "" (
			set str=!str:template=%name%!
		)
		echo,!str!
	)
) > temp%name%.vcxproj.user
move /y temp%name%.vcxproj.user %name%.vcxproj.user
cd ..\..\

pause
