dependencies:
    1. openssl[static libs]
        vcpkg install openssl:x64-windows-static
        ./vcpkg install openssl:x64-linux

    2. hiredis[static libs]
        vcpkg install hiredis:x64-windows-static
        ./vcpkg install hiredis:x64-linux

    3. asio with openssl

    4. spdlog with fmt

    5. date