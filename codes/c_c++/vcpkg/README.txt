git submodule:
    cd helinljn
    git submodule add https://github.com/microsoft/vcpkg.git codes/c_c++/vcpkg/vcpkg

Dependencies:
    1. static libs
        git checkout 3b3bd424827a1f7f4813216f6b32b6c61e386b2e
            Windows:
                bootstrap-vcpkg.bat

                # 1.1.1n
                vcpkg install openssl:x64-windows-static

                # 2.1.12
                vcpkg install libevent[openssl]:x64-windows-static

                # 1.0.2
                vcpkg install hiredis[ssl]:x64-windows-static
            Linux:
                ./bootstrap-vcpkg.sh

                # 1.1.1n
                ./vcpkg install openssl:x64-linux

                # 2.1.12
                ./vcpkg install libevent[openssl]:x64-linux

                # 1.0.2
                ./vcpkg install hiredis[ssl]:x64-linux

        git checkout 2022.05.10
            Windows:
                bootstrap-vcpkg.bat
            Linux:
                ./bootstrap-vcpkg.sh