# 项目名字
PROJECT(test)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/googletest/googlemock/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/googletest/googletest/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/JSON/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/JWT/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/NetSSL_OpenSSL/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Zip/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-build/poco/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
    ${CMAKE_PROJECT_ROOT_DIR}/src/common
    ${CMAKE_3RD_DIR_MYSQL}/include/mysql
    ${CMAKE_3RD_DIR_OPENSSL}/include
)

# 链接库目录
SET(CURRENT_LINK_LIBS_DIR
    ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
    ${CMAKE_3RD_DIR_MYSQL}/lib
    ${CMAKE_3RD_DIR_OPENSSL}/lib
)

# 生成依赖库
SET(CURRENT_DEPENDENT_LIBS
    common
    protoc
    protobuf
    gmock
    gtest
    fmt
    PocoRedis
    PocoDataMySQL
    PocoData
    PocoNetSSL
    PocoNet
    PocoUtil
    PocoZip
    PocoXML
    PocoJWT
    PocoJSON
    PocoCrypto
    PocoFoundation
)

# 链接库、宏定义、编译选项
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 链接库
    SET(CURRENT_LINK_LIBS
        ${CURRENT_DEPENDENT_LIBS}
        libmysql
        libssl
        libcrypto
        crypt32
        ws2_32
        iphlpapi
        dbghelp
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4819
    )
ELSE()
    # 链接库
    SET(CURRENT_LINK_LIBS
        ${CURRENT_DEPENDENT_LIBS}
        mysqlclient
        ssl
        crypto
        pthread
        m
        dl
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

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${PROJECT_SOURCE_DIR}/*.h
    ${PROJECT_SOURCE_DIR}/*.hpp
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${PROJECT_SOURCE_DIR}/*.c
    ${PROJECT_SOURCE_DIR}/*.cc
    ${PROJECT_SOURCE_DIR}/*.cpp
)

# 生成目标可执行文件
ADD_EXECUTABLE(${PROJECT_NAME}                     ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
ADD_DEPENDENCIES(${PROJECT_NAME}                   ${CURRENT_DEPENDENT_LIBS})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PRIVATE ${CURRENT_LINK_LIBS})

# VS工程设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "projects")
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}")

    SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ELSE()
    # Linux平台下rpath动态库运行路径修改，优先查找当前目录下的动态库
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND patchelf --set-rpath ./
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}
    )
ENDIF()