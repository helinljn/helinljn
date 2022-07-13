git submodule:
	cd helinljn
	git submodule add https://github.com/microsoft/vcpkg.git codes/c_c++/vcpkg/vcpkg

git config:
	Windows:
        git config --global core.autocrlf true
    Linux or Mac:
        git config --global core.autocrlf input
	Windows only:
		git config --global core.autocrlf false