# 项目名字
PROJECT(hiredis)

# 将编译目标添加至依赖列表，用于生成可执行文件时的编译依赖
SET(CURRENT_DEPENDENT_LIBS_LIST "${CURRENT_DEPENDENT_LIBS_LIST};${PROJECT_NAME}" CACHE STRING INTERNAL FORCE)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-windows/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DMICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS=0
        -DWIN32_LEAN_AND_MEAN
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4102
        /wd4146
        /wd4189
        /wd4200
        /wd4244
        /wd4267
        /wd4312
        /wd4456
        /wd4702
        /wd4996
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-ubuntu-20.04/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-unused-variable
        -Wno-unused-function
    )
ENDIF()

# 源文件
SET(CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/alloc.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/async.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/dict.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/hiredis.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/net.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/read.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/sds.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/sockcompat.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/ssl.c
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

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