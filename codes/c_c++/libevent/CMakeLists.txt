# CMake最低版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

# C/C++语言
SET(CMAKE_C_STANDARD            99)
SET(CMAKE_C_STANDARD_REQUIRED   ON)
SET(CMAKE_C_EXTENSIONS          OFF)
SET(CMAKE_CXX_STANDARD          17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS        OFF)

# Debug & Release选项
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Debug or Release" FORCE)
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is not set, set default build to Debug")
ENDIF()

# 项目名字
PROJECT(event_all)

# 生成目标名字
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    SET(BUILD_TARGET_NAME "${CMAKE_PROJECT_NAME}_d")
ELSE()
    SET(BUILD_TARGET_NAME "${CMAKE_PROJECT_NAME}_r")
ENDIF()

# 头文件目录、源文件、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(LIBEVENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/installed/x64-windows-static/include
    )

    # 源文件
    SET(LIBEVENT_SRC_LIST
        ${PROJECT_SOURCE_DIR}/src/buffer.c
        ${PROJECT_SOURCE_DIR}/src/buffer_iocp.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_async.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_filter.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_openssl.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_pair.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_ratelim.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_sock.c
        ${PROJECT_SOURCE_DIR}/src/evdns.c
        ${PROJECT_SOURCE_DIR}/src/event.c
        ${PROJECT_SOURCE_DIR}/src/event_iocp.c
        ${PROJECT_SOURCE_DIR}/src/event_tagging.c
        ${PROJECT_SOURCE_DIR}/src/evmap.c
        ${PROJECT_SOURCE_DIR}/src/evrpc.c
        ${PROJECT_SOURCE_DIR}/src/evthread.c
        ${PROJECT_SOURCE_DIR}/src/evthread_win32.c
        ${PROJECT_SOURCE_DIR}/src/evutil.c
        ${PROJECT_SOURCE_DIR}/src/evutil_rand.c
        ${PROJECT_SOURCE_DIR}/src/evutil_time.c
        ${PROJECT_SOURCE_DIR}/src/http.c
        ${PROJECT_SOURCE_DIR}/src/listener.c
        ${PROJECT_SOURCE_DIR}/src/log.c
        ${PROJECT_SOURCE_DIR}/src/signal.c
        ${PROJECT_SOURCE_DIR}/src/strlcpy.c
        ${PROJECT_SOURCE_DIR}/src/win32select.c
    )

    # 宏定义
    ADD_DEFINITIONS(
        -DXXX_VALUE_JUST_FOR_TEST=818
        -D_WIN32_WINNT=0x0601
        # ...
    )

    # 编译选项
    SET(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /W4 /MP /wd4100 /wd4152 /wd4244 /wd4245 /wd4267 /wd4706 /wd4996")
    SET(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}")
    SET(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}")

    SET(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus /W4 /MP")
    SET(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

    # 设置MTd & MT
    SET(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
ELSE()
    # 头文件目录
    SET(LIBEVENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/installed/x64-linux/include
    )

    # 源文件
    SET(LIBEVENT_SRC_LIST
        ${PROJECT_SOURCE_DIR}/src/buffer.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_filter.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_openssl.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_pair.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_ratelim.c
        ${PROJECT_SOURCE_DIR}/src/bufferevent_sock.c
        ${PROJECT_SOURCE_DIR}/src/epoll.c
        ${PROJECT_SOURCE_DIR}/src/evdns.c
        ${PROJECT_SOURCE_DIR}/src/event.c
        ${PROJECT_SOURCE_DIR}/src/event_tagging.c
        ${PROJECT_SOURCE_DIR}/src/evmap.c
        ${PROJECT_SOURCE_DIR}/src/evrpc.c
        ${PROJECT_SOURCE_DIR}/src/evthread.c
        ${PROJECT_SOURCE_DIR}/src/evthread_pthread.c
        ${PROJECT_SOURCE_DIR}/src/evutil.c
        ${PROJECT_SOURCE_DIR}/src/evutil_rand.c
        ${PROJECT_SOURCE_DIR}/src/evutil_time.c
        ${PROJECT_SOURCE_DIR}/src/http.c
        ${PROJECT_SOURCE_DIR}/src/listener.c
        ${PROJECT_SOURCE_DIR}/src/log.c
        ${PROJECT_SOURCE_DIR}/src/poll.c
        ${PROJECT_SOURCE_DIR}/src/select.c
        ${PROJECT_SOURCE_DIR}/src/signal.c
        ${PROJECT_SOURCE_DIR}/src/strlcpy.c
    )

    # 宏定义
    ADD_DEFINITIONS(
        -DXXX_VALUE_JUST_FOR_TEST=818
        # ...
    )

    # 编译选项
    SET(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -Wpedantic -Wno-format -Wno-unused-parameter -Wno-unused-value")
    SET(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}")
    SET(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}")

    SET(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wpedantic")
    SET(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
ENDIF()

MESSAGE(STATUS "---------------------------------------------------------------------")

# 设置生成目录
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)

# 生成静态库
ADD_LIBRARY(${BUILD_TARGET_NAME} STATIC ${LIBEVENT_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${BUILD_TARGET_NAME} PRIVATE ${LIBEVENT_INCLUDE_DIR})

MESSAGE(STATUS "LIBEVENT_SRC_LIST    = ${LIBEVENT_SRC_LIST}")
MESSAGE(STATUS "LIBEVENT_INCLUDE_DIR = ${LIBEVENT_INCLUDE_DIR}")

# 调试信息
MESSAGE(STATUS "---------------------------------------------------------------------")
MESSAGE(STATUS "PROJECT_SOURCE_DIR             = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR             = ${PROJECT_BINARY_DIR}")
MESSAGE(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
MESSAGE(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
MESSAGE(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
MESSAGE(STATUS "CMAKE_PROJECT_NAME             = ${CMAKE_PROJECT_NAME}")
MESSAGE(STATUS "BUILD_TARGET_NAME              = ${BUILD_TARGET_NAME}")
MESSAGE(STATUS "CMAKE_C_STANDARD               = C${CMAKE_C_STANDARD}")
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