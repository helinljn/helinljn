# 项目名字
PROJECT(spdlog)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DSPDLOG_FMT_EXTERNAL
        -DSPDLOG_COMPILED_LIB
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DSPDLOG_FMT_EXTERNAL
        -DSPDLOG_COMPILED_LIB
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ENDIF()

# 源文件
SET(CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/src/cfg.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/src/async.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/src/spdlog.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/src/file_sinks.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/src/color_sinks.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/src/stdout_sinks.cpp
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})