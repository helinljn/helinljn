git submodule:
	cd helinljn
	git submodule add https://github.com/redis/hiredis.git codes/c_c++/hiredis/hiredis

git config:
	Windows:
        git config --global core.autocrlf true
    Linux or Mac:
        git config --global core.autocrlf input
	Windows only:
		git config --global core.autocrlf false