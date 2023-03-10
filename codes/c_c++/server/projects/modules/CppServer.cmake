# 项目名字
PROJECT(cppserver)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/asio/asio/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/CppCommon/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/include
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-windows/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/asio/asio/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/CppCommon/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/include
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-ubuntu-20.04/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-unused-parameter
    )
ENDIF()

# 递归添加[${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/source]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/source CURRENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/source/*.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/source/*.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/source/*.cpp
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})