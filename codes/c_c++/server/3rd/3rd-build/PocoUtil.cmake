# 项目名字
PROJECT(PocoUtil)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/JSON/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include
)

# 宏定义、编译选项
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-sign-compare
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include/*.h
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/*.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/*.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/*.cpp
)

# 排除特定平台实现源文件
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    GET_FILENAME_COMPONENT(
        WinRegistryConfiguration
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/WinRegistryConfiguration.cpp
        ABSOLUTE)
    GET_FILENAME_COMPONENT(
        WinRegistryKey
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/WinRegistryKey.cpp
        ABSOLUTE)
    GET_FILENAME_COMPONENT(
        WinService
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/WinService.cpp
        ABSOLUTE)

    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${WinRegistryConfiguration})
    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${WinRegistryKey})
    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${WinService})
ENDIF()

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PUBLIC  PocoXML PocoJSON PocoFoundation)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 单独为Windows下C++源文件设置额外的编译选项
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: /Zc:strictStrings->)

    # 生成VS工程目录
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP("src"
        FILES ${CURRENT_DIR_SRC_LIST})
ENDIF()