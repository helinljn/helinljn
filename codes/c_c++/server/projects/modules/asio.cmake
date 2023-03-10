# 项目名字
PROJECT(asio)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/asio/asio/include
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-windows/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DASIO_STANDALONE
        -DASIO_SEPARATE_COMPILATION
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/asio/asio/include
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-ubuntu-20.04/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DASIO_STANDALONE
        -DASIO_SEPARATE_COMPILATION
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ENDIF()

# 源文件
SET(CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/asio/asio/src/asio.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/asio/asio/src/asio_ssl.cpp
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})