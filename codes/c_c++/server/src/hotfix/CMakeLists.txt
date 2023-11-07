# 项目名字
PROJECT(hotfix)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_3RD_DIR_MYSQL}/include
    ${CMAKE_3RD_DIR_OPENSSL}/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/JSON/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/JWT/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Zip/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/NetSSL_OpenSSL/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
    ${CMAKE_PROJECT_ROOT_DIR}/src/common
)

# 链接库目录
SET(CURRENT_LINK_LIBS_DIR
    ${CMAKE_3RD_DIR_MYSQL}/lib
    ${CMAKE_3RD_DIR_OPENSSL}/lib
    ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
)

# 宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DHOTFIX_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        common
        protobuf
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
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DHOTFIX_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        common
        protobuf
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

# 生成动态库
ADD_LIBRARY(${PROJECT_NAME}                SHARED  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PUBLIC  ${CURRENT_LINK_LIBS})

# 其它设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # MSVC运行库设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # VS工程设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "projects")

    SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ELSE()
    # Linux平台下rpath动态库运行路径修改，优先查找当前目录下的动态库
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND patchelf --set-rpath ./
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/lib${PROJECT_NAME}.so
    )
ENDIF()