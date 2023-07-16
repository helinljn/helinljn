# 项目名字
PROJECT(PocoFoundation)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/src
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-build/poco/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4127
        /wd4189
        /wd4244
        /wd4267
        /wd4456
        /wd4457
        /wd4459
        /wd4702
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/src
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-build/poco/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-pedantic
        -Wno-type-limits
        -Wno-sign-compare
        -Wno-unused-parameter
        -Wno-deprecated-copy
        -Wno-ignored-qualifiers
        -Wno-implicit-fallthrough
    )
ENDIF()

# 递归添加[${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/src]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/src CURRENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/src/*.cpp
)

# 排除平台实现相关源文件*_*.cpp
FOREACH(TEMP_FILE ${CURRENT_DIR_SRC_LIST})
    GET_FILENAME_COMPONENT(TEMP_NAME ${TEMP_FILE} NAME)

    IF(NOT (${TEMP_NAME} MATCHES ".*_.*"))
        LIST(APPEND TEMP_SRC_LIST "${TEMP_FILE}")
    ENDIF()
ENDFOREACH()

# 用排除了平台实现相关源文件之后的列表覆盖当前源文件列表
SET(CURRENT_DIR_SRC_LIST ${TEMP_SRC_LIST})

# 排除特定平台实现源文件
IF(MSVC)
    GET_FILENAME_COMPONENT(
        SyslogChannel
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/src/SyslogChannel.cpp
        ABSOLUTE)

    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${SyslogChannel})
ELSE()
    GET_FILENAME_COMPONENT(
        EventLogChannel
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/src/EventLogChannel.cpp
        ABSOLUTE)
    GET_FILENAME_COMPONENT(
        WindowsConsoleChannel
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/src/WindowsConsoleChannel.cpp
        ABSOLUTE)

    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${EventLogChannel})
    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${WindowsConsoleChannel})
ENDIF()

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})