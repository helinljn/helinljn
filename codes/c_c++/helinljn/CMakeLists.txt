# CMake最低版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

# C++语言设置
SET(CMAKE_CXX_STANDARD          17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS        OFF)
SET(CMAKE_VERBOSE_MAKEFILE      OFF)

# 设置Debug或者Release
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Debug or Release" FORCE)
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is not set, set default build to Debug")
ENDIF()

# 项目名字
PROJECT(helinljn)

# 生成目标名字
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    SET(BUILD_TARGET_NAME "${CMAKE_PROJECT_NAME}_d")
ELSE()
    SET(BUILD_TARGET_NAME "${CMAKE_PROJECT_NAME}_r")
ENDIF()

# 添加头文件、依赖库、宏定义、编译选项
IF(MSVC)
    # 公共头文件
    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/installed/x64-windows-static/include
        ${PROJECT_SOURCE_DIR}/../asio/asio/asio/include
        ${PROJECT_SOURCE_DIR}/../spdlog/spdlog/include
        ${PROJECT_SOURCE_DIR}/../fmt/fmt/include
    )

    # 公共依赖库路径
    LINK_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/installed/x64-windows-static/lib
    )

    # 公共依赖库(优先链接静态库，然后才是动态库)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll ${CMAKE_FIND_LIBRARY_SUFFIXES})
    SET(PUBLIC_LINK_LIBS
        event
        event_core
        event_extra
        event_openssl
        boost_atomic-vc140-mt
        boost_chrono-vc140-mt
        boost_container-vc140-mt
        boost_date_time-vc140-mt
        boost_exception-vc140-mt
        boost_filesystem-vc140-mt
        boost_random-vc140-mt
        boost_regex-vc140-mt
        boost_system-vc140-mt
        boost_thread-vc140-mt
        boost_timer-vc140-mt
        libssl
        libcrypto
        crypt32
    )

    # 宏定义
    ADD_DEFINITIONS(
        -DXXX_VALUE_JUST_FOR_TEST=818
        -D_WIN32_WINNT=0x0601
        -DASIO_STANDALONE
        -DFMT_HEADER_ONLY
        -DSPDLOG_FMT_EXTERNAL
        # ...
    )

    # 编译选项
    SET(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS}")
    SET(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}")
    SET(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}")

    SET(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus /W4 /MP")
    SET(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

    # 设置MTd & MT
    SET(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
ELSE()
    # 公共头文件
    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/installed/x64-linux/include
        ${PROJECT_SOURCE_DIR}/../asio/asio/asio/include
        ${PROJECT_SOURCE_DIR}/../spdlog/spdlog/include
        ${PROJECT_SOURCE_DIR}/../fmt/fmt/include
    )

    # 公共依赖库路径
    LINK_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/installed/x64-linux/lib
    )

    # 公共依赖库(优先链接静态库，然后才是动态库)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    SET(PUBLIC_LINK_LIBS
        -levent
        -levent_core
        -levent_extra
        -levent_openssl
        -levent_pthreads
        -lboost_atomic
        -lboost_chrono
        -lboost_container
        -lboost_date_time
        -lboost_exception
        -lboost_filesystem
        -lboost_random
        -lboost_regex
        -lboost_system
        -lboost_thread
        -lboost_timer
        -lssl
        -lcrypto
        -lstdc++fs
        -lpthread
        -ldl
    )

    # 宏定义
    ADD_DEFINITIONS(
        -DXXX_VALUE_JUST_FOR_TEST=818
        -DASIO_STANDALONE
        -DFMT_HEADER_ONLY
        -DSPDLOG_FMT_EXTERNAL
        # ...
    )

    # 编译选项
    SET(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS}")
    SET(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}")
    SET(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}")

    SET(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wpedantic")
    SET(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
ENDIF()

MESSAGE(STATUS "---------------------------------------------------------------------")

# 添加[./test]目录下所有源文件(不包括头文件)
AUX_SOURCE_DIRECTORY(./test SRC_LIST_TEST)
MESSAGE(STATUS "SRC_LIST_TEST = ${SRC_LIST_TEST}")

# 添加[./util]目录下所有源文件(不包括头文件)
AUX_SOURCE_DIRECTORY(./util SRC_LIST_UTIL)
MESSAGE(STATUS "SRC_LIST_UTIL = ${SRC_LIST_UTIL}")

# 生成目标可执行文件
ADD_EXECUTABLE(${BUILD_TARGET_NAME} ${SRC_LIST_TEST} ${SRC_LIST_UTIL})

# 链接公共依赖库
TARGET_LINK_LIBRARIES(${BUILD_TARGET_NAME} ${PUBLIC_LINK_LIBS})
MESSAGE(STATUS "PUBLIC_LINK_LIBS = ${PUBLIC_LINK_LIBS}")

# 调试信息
MESSAGE(STATUS "---------------------------------------------------------------------")
MESSAGE(STATUS "PROJECT_SOURCE_DIR             = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR             = ${PROJECT_BINARY_DIR}")
MESSAGE(STATUS "CMAKE_PROJECT_NAME             = ${CMAKE_PROJECT_NAME}")
MESSAGE(STATUS "BUILD_TARGET_NAME              = ${BUILD_TARGET_NAME}")
MESSAGE(STATUS "CMAKE_CXX_STANDARD             = C++${CMAKE_CXX_STANDARD}")
MESSAGE(STATUS "CMAKE_BUILD_TYPE               = ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "CMAKE_FIND_LIBRARY_SUFFIXES    = ${CMAKE_FIND_LIBRARY_SUFFIXES}")
MESSAGE(STATUS "CMAKE_C_FLAGS                  = ${CMAKE_C_FLAGS}")
MESSAGE(STATUS "CMAKE_C_FLAGS_DEBUG            = ${CMAKE_C_FLAGS_DEBUG}")
MESSAGE(STATUS "CMAKE_C_FLAGS_RELEASE          = ${CMAKE_C_FLAGS_RELEASE}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS                = ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS_DEBUG          = ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS_RELEASE        = ${CMAKE_CXX_FLAGS_RELEASE}")
MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS         = ${CMAKE_EXE_LINKER_FLAGS}")
MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS_DEBUG   = ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
MESSAGE(STATUS "CMAKE_EXE_LINKER_FLAGS_RELEASE = ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
MESSAGE(STATUS "---------------------------------------------------------------------")