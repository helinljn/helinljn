# 项目名字
PROJECT(test)

# 头文件目录、依赖库目录、依赖库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Foundation/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/XML/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/XML/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/JSON/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/JSON/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Util/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Util/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Net/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Net/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Crypto/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Crypto/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/NetSSL_OpenSSL/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/NetSSL_OpenSSL/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/JWT/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/JWT/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Redis/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Redis/src
        ${CMAKE_PROJECT_ROOT_DIR}/../vcpkg/vcpkg/packages/openssl_x64-windows-static/include
    )

    # 依赖库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/poco/lib64
        ${CMAKE_PROJECT_ROOT_DIR}/../vcpkg/vcpkg/packages/openssl_x64-windows-static/lib
    )

    # 依赖库后缀修正
    IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        SET(CURRENT_POCO_LIBS_SUFFIX "mtd")
    ELSE()
        SET(CURRENT_POCO_LIBS_SUFFIX "mt")
    ENDIF()

    # 依赖库
    SET(CURRENT_LINK_LIBS
        PocoFoundation${CURRENT_POCO_LIBS_SUFFIX}
        PocoXML${CURRENT_POCO_LIBS_SUFFIX}
        PocoJSON${CURRENT_POCO_LIBS_SUFFIX}
        PocoUtil${CURRENT_POCO_LIBS_SUFFIX}
        PocoNet${CURRENT_POCO_LIBS_SUFFIX}
        PocoCrypto${CURRENT_POCO_LIBS_SUFFIX}
        PocoNetSSL${CURRENT_POCO_LIBS_SUFFIX}
        PocoJWT${CURRENT_POCO_LIBS_SUFFIX}
        PocoRedis${CURRENT_POCO_LIBS_SUFFIX}
        libssl
        libcrypto
        ws2_32
        crypt32
        iphlpapi
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Foundation/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/XML/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/XML/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/JSON/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/JSON/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Util/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Util/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Net/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Net/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Crypto/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Crypto/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/NetSSL_OpenSSL/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/NetSSL_OpenSSL/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/JWT/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/JWT/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Redis/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Redis/src
        ${CMAKE_PROJECT_ROOT_DIR}/../vcpkg/vcpkg/packages/openssl_x64-linux/include
    )

    # 依赖库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/poco/lib64
        ${CMAKE_PROJECT_ROOT_DIR}/../vcpkg/vcpkg/packages/openssl_x64-linux/lib
    )

    # 依赖库后缀修正
    IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        SET(CURRENT_POCO_LIBS_SUFFIX "d")
    ELSE()
        SET(CURRENT_POCO_LIBS_SUFFIX "")
    ENDIF()

    # 依赖库
    SET(CURRENT_LINK_LIBS
        -lPocoFoundation${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoXML${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoJSON${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoUtil${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoNet${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoCrypto${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoNetSSL${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoJWT${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoRedis${CURRENT_POCO_LIBS_SUFFIX}
        -lssl
        -lcrypto
        -lpthread
        -lm
        -ldl
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ENDIF()

# 递归添加[./]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(./ CURRENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST ./*.c ./*.cc ./*.cpp)

# 生成目标可执行文件
ADD_EXECUTABLE(${PROJECT_NAME}                     ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PRIVATE ${CURRENT_LINK_LIBS})

# 调试信息
MESSAGE(STATUS "---------------------------------------------------------------------")
MESSAGE(STATUS "CMAKE_PROJECT_NAME          = ${CMAKE_PROJECT_NAME}")
MESSAGE(STATUS "PROJECT_NAME                = ${PROJECT_NAME}")
MESSAGE(STATUS "PROJECT_SOURCE_DIR          = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR          = ${PROJECT_BINARY_DIR}")
MESSAGE(STATUS "CURRENT_DIR_SRC_LIST        = ${CURRENT_DIR_SRC_LIST}")
MESSAGE(STATUS "CURRENT_INCLUDE_DIR         = ${CURRENT_INCLUDE_DIR}")
MESSAGE(STATUS "CURRENT_COMPILE_DEFINITIONS = ${CURRENT_COMPILE_DEFINITIONS}")
MESSAGE(STATUS "CURRENT_COMPILE_OPTIONS     = ${CURRENT_COMPILE_OPTIONS}")
MESSAGE(STATUS "CURRENT_LINK_LIBS_DIR       = ${CURRENT_LINK_LIBS_DIR}")
MESSAGE(STATUS "CURRENT_LINK_LIBS           = ${CURRENT_LINK_LIBS}")