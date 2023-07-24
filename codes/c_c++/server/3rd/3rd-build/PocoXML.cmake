# 项目名字
PROJECT(PocoXML)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/src
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DPOCO_NO_AUTOMATIC_LIBS
        -DPOCO_NO_AUTOMATIC_LIB_INIT
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4127
        /wd4206
        /wd4456
        /wd4457
        /wd4505
        /wd4551
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/src
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DPOCO_NO_AUTOMATIC_LIBS
        -DPOCO_NO_AUTOMATIC_LIB_INIT
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-pedantic
        -Wno-unused-parameter
        -Wno-implicit-fallthrough
    )
ENDIF()

# 递归添加[${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/src]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/src CURRENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/src/*.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/src/*.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/src/*.cpp
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})