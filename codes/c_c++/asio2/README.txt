git submodule:
	cd helinljn
	git submodule add https://github.com/zhllxt/asio2.git codes/c_c++/asio2/asio2

git config:
	Windows:
        git config --global core.autocrlf true
    Linux or Mac:
        git config --global core.autocrlf input
	Windows only:
		git config --global core.autocrlf false