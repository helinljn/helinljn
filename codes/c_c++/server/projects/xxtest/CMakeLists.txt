# 项目名字
PROJECT(xxtest)

# 头文件目录、链接库目录、生成依赖库、链接库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/googletest/googlemock/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/googletest/googletest/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/compat
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-build/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/JSON/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/NetSSL_OpenSSL/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Zip/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-build/poco/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/mysql-8.0.33/x64-windows/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/openssl-1.1.1t/x64-windows/include
        ${CMAKE_PROJECT_ROOT_DIR}/projects/xxprotocol
    )

    # 链接库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/mysql-8.0.33/x64-windows/lib
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/openssl-1.1.1t/x64-windows/lib
    )

    # 生成依赖库
    SET(CURRENT_DEPENDENT_LIBS
        xxprotocol
        protobuf
        gmock
        gtest
        event
        spdlog
        fmt
        PocoDataMySQL
        PocoData
        PocoRedis
        PocoNetSSL
        PocoNet
        PocoUtil
        PocoZip
        PocoXML
        PocoJSON
        PocoCrypto
        PocoFoundation
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        ${CURRENT_DEPENDENT_LIBS}
        libssl
        libcrypto
        libmysql
        ws2_32
        crypt32
        iphlpapi
        dbghelp
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DPOCO_NO_AUTOMATIC_LIBS
        -DPOCO_NO_AUTOMATIC_LIB_INIT
        -DSPDLOG_FMT_EXTERNAL
        -DSPDLOG_COMPILED_LIB
        -DXML_STATIC
        -DXML_DTD
        -DXML_NS
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4819
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/googletest/googlemock/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/googletest/googletest/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/compat
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-build/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/JSON/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/NetSSL_OpenSSL/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Zip/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-build/poco/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/mysql-8.0.33/x64-ubuntu-20.04/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/openssl-1.1.1t/x64-ubuntu-20.04/include
        ${CMAKE_PROJECT_ROOT_DIR}/projects/xxprotocol
    )

    # 链接库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/mysql-8.0.33/x64-ubuntu-20.04/lib
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/openssl-1.1.1t/x64-ubuntu-20.04/lib
    )

    # 生成依赖库
    SET(CURRENT_DEPENDENT_LIBS
        xxprotocol
        protobuf
        gmock
        gtest
        event
        spdlog
        fmt
        PocoDataMySQL
        PocoData
        PocoRedis
        PocoNetSSL
        PocoNet
        PocoUtil
        PocoZip
        PocoXML
        PocoJSON
        PocoCrypto
        PocoFoundation
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        ${CURRENT_DEPENDENT_LIBS}
        ssl
        crypto
        mysqlclient
        pthread
        m
        dl
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DPOCO_NO_AUTOMATIC_LIBS
        -DPOCO_NO_AUTOMATIC_LIB_INIT
        -DSPDLOG_FMT_EXTERNAL
        -DSPDLOG_COMPILED_LIB
        -DXML_STATIC
        -DXML_DTD
        -DXML_NS
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ENDIF()

# 递归添加[./]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(./ CURRENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ./*.c
    ./*.cc
    ./*.cpp
)

# 生成目标可执行文件
ADD_EXECUTABLE(${PROJECT_NAME}                     ${CURRENT_DIR_SRC_LIST})
ADD_DEPENDENCIES(${PROJECT_NAME}                   ${CURRENT_DEPENDENT_LIBS})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PRIVATE ${CURRENT_LINK_LIBS})

# Windows平台下的MySQL和OpenSSL使用的都是动态库，所以需要将其对应的动态库拷贝至生成目录
IF(MSVC)
    ADD_CUSTOM_COMMAND(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/mysql-8.0.33/x64-windows/lib/libmysql.dll ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/openssl-1.1.1t/x64-windows/bin/libssl-1_1-x64.dll ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-libs/openssl-1.1.1t/x64-windows/bin/libcrypto-1_1-x64.dll ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
    )
ENDIF()