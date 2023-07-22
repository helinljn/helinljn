# 项目名字
PROJECT(PocoNet)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/src
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DPOCO_NO_AUTOMATIC_LIBS
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4127
        /wd4244
        /wd4245
        /wd4267
        /wd4389
        /wd4456
        /wd4457
        /wd4458
        /wd4702
        /wd4706
        /wd5105
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/src
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DPOCO_NO_AUTOMATIC_LIBS
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-extra
        -Wno-sign-compare
        -Wno-unused-result
        -Wno-unused-parameter
    )
ENDIF()

# 递归添加[${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/src]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/src CURRENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/src/*.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/src/*.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/src/*.cpp
)

# 排除特定平台实现源文件
IF(UNIX)
    GET_FILENAME_COMPONENT(
        wepoll
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/src/wepoll.c
        ABSOLUTE)

    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${wepoll})
ENDIF()

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

# 单独为Windows下C++源文件设置额外的编译选项
IF(MSVC)
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: /Zc:strictStrings->)
ENDIF()