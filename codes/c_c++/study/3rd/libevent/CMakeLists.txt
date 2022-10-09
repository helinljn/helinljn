# 项目名字
PROJECT(event)

# 将编译目标添加至依赖列表，用于生成可执行文件时的编译依赖
SET(CURRENT_DEPENDENT_LIBS_LIST "${CURRENT_DEPENDENT_LIBS_LIST};${PROJECT_NAME}" CACHE STRING INTERNAL FORCE)

# 头文件目录、源文件、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../../../vcpkg/vcpkg/packages/openssl_x64-windows-static/include
    )

    # 源文件
    SET(CURRENT_DIR_SRC_LIST
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
    SET(CURRENT_COMPILE_DEFINITIONS
        -DMICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS=0
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4115
        /wd4127
        /wd4131
        /wd4152
        /wd4244
        /wd4245
        /wd4267
        /wd4295
        /wd4456
        /wd4457
        /wd4702
        /wd4706
        /wd4819
        /wd4996
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../../../vcpkg/vcpkg/packages/openssl_x64-linux/include
    )

    # 源文件
    SET(CURRENT_DIR_SRC_LIST
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
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-format
        -Wno-unused-value
        -Wno-unused-parameter
    )
ENDIF()

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