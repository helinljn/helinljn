# 项目名字
PROJECT(test)

# 头文件目录、链接库目录、链接库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Foundation/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Crypto/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Crypto/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Zip/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Zip/src
        ${CMAKE_PROJECT_ROOT_DIR}/../server/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/../server/3rd/3rd-libs/openssl-1.1.1t/x64-windows/include
    )

    # 链接库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/poco/lib64
        ${CMAKE_PROJECT_ROOT_DIR}/../server/3rd/3rd-libs/openssl-1.1.1t/x64-windows/lib
    )

    # 链接库后缀修正
    IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        SET(CURRENT_POCO_LIBS_SUFFIX "mtd")
    ELSE()
        SET(CURRENT_POCO_LIBS_SUFFIX "mt")
    ENDIF()

    # 链接库
    SET(CURRENT_LINK_LIBS
        PocoFoundation${CURRENT_POCO_LIBS_SUFFIX}
        PocoCrypto${CURRENT_POCO_LIBS_SUFFIX}
        PocoZip${CURRENT_POCO_LIBS_SUFFIX}
        libssl
        libcrypto
        ws2_32
        crypt32
        iphlpapi
        dbghelp
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DPOCO_NO_AUTOMATIC_LIBS
        -DFMT_HEADER_ONLY
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4819
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Foundation/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Crypto/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Crypto/src
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Zip/include
        ${CMAKE_PROJECT_ROOT_DIR}/poco/Zip/src
        ${CMAKE_PROJECT_ROOT_DIR}/../server/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/../server/3rd/3rd-libs/openssl-1.1.1t/x64-ubuntu-20.04/include
    )

    # 链接库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/poco/lib64
        ${CMAKE_PROJECT_ROOT_DIR}/../server/3rd/3rd-libs/openssl-1.1.1t/x64-ubuntu-20.04/lib
    )

    # 链接库后缀修正
    IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        SET(CURRENT_POCO_LIBS_SUFFIX "d")
    ELSE()
        SET(CURRENT_POCO_LIBS_SUFFIX "")
    ENDIF()

    # 链接库
    SET(CURRENT_LINK_LIBS
        -lPocoFoundation${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoCrypto${CURRENT_POCO_LIBS_SUFFIX}
        -lPocoZip${CURRENT_POCO_LIBS_SUFFIX}
        -lssl
        -lcrypto
        -lpthread
        -lm
        -ldl
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DPOCO_NO_AUTOMATIC_LIBS
        -DFMT_HEADER_ONLY
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
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PRIVATE ${CURRENT_LINK_LIBS})