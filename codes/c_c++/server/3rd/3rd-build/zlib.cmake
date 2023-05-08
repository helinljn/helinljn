# 项目名字
PROJECT(zlib)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4127
        /wd4131
        /wd4244
        /wd4996
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-implicit-function-declaration
    )
ENDIF()

# 源文件
SET(CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/adler32.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/compress.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/crc32.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/deflate.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/gzclose.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/gzlib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/gzread.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/gzwrite.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/infback.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/inffast.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/inflate.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/inftrees.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/trees.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/uncompr.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib/zutil.c
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})