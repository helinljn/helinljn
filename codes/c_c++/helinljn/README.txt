Dependencies:
    1. static libs
        Windows:
            vcpkg install openssl:x64-windows-static

            vcpkg install libevent[openssl]:x64-windows-static

            vcpkg install boost-algorithm:x64-windows-static
            vcpkg install boost-align:x64-windows-static
            vcpkg install boost-any:x64-windows-static
            vcpkg install boost-atomic:x64-windows-static
            vcpkg install boost-chrono:x64-windows-static
            vcpkg install boost-circular-buffer:x64-windows-static
            vcpkg install boost-crc:x64-windows-static
            vcpkg install boost-date-time:x64-windows-static
            vcpkg install boost-endian:x64-windows-static
            vcpkg install boost-filesystem:x64-windows-static
            vcpkg install boost-hana:x64-windows-static
            vcpkg install boost-heap:x64-windows-static
            vcpkg install boost-interprocess:x64-windows-static
            vcpkg install boost-random:x64-windows-static
            vcpkg install boost-sort:x64-windows-static
            vcpkg install boost-static-string:x64-windows-static
            vcpkg install boost-thread:x64-windows-static
            vcpkg install boost-timer:x64-windows-static

        Linux:
            ./vcpkg install openssl:x64-linux

            ./vcpkg install libevent[openssl]:x64-linux

            ./vcpkg install boost-algorithm:x64-linux
            ./vcpkg install boost-align:x64-linux
            ./vcpkg install boost-any:x64-linux
            ./vcpkg install boost-atomic:x64-linux
            ./vcpkg install boost-chrono:x64-linux
            ./vcpkg install boost-circular-buffer:x64-linux
            ./vcpkg install boost-crc:x64-linux
            ./vcpkg install boost-date-time:x64-linux
            ./vcpkg install boost-endian:x64-linux
            ./vcpkg install boost-filesystem:x64-linux
            ./vcpkg install boost-hana:x64-linux
            ./vcpkg install boost-heap:x64-linux
            ./vcpkg install boost-interprocess:x64-linux
            ./vcpkg install boost-random:x64-linux
            ./vcpkg install boost-sort:x64-linux
            ./vcpkg install boost-static-string:x64-linux
            ./vcpkg install boost-thread:x64-linux
            ./vcpkg install boost-timer:x64-linux

    2. asio with openssl

    3. fmt

    4. spdlog