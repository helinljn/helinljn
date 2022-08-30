# 生成目标名字
SET(LIBRARY_LIBEVENT_BUILD_TARGET_NAME "event_all")

# 头文件目录、源文件、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(LIBRARY_LIBEVENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/3rd/libevent
        ${PROJECT_SOURCE_DIR}/3rd/libevent/include
        ${PROJECT_SOURCE_DIR}/3rd/libevent/src
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-windows-static/include
    )

    # 源文件
    SET(LIBRARY_LIBEVENT_DIR_SRC_LIST
        ./src/buffer.c
        ./src/buffer_iocp.c
        ./src/bufferevent.c
        ./src/bufferevent_async.c
        ./src/bufferevent_filter.c
        ./src/bufferevent_openssl.c
        ./src/bufferevent_pair.c
        ./src/bufferevent_ratelim.c
        ./src/bufferevent_sock.c
        ./src/evdns.c
        ./src/event.c
        ./src/event_iocp.c
        ./src/event_tagging.c
        ./src/evmap.c
        ./src/evrpc.c
        ./src/evthread.c
        ./src/evthread_win32.c
        ./src/evutil.c
        ./src/evutil_rand.c
        ./src/evutil_time.c
        ./src/http.c
        ./src/listener.c
        ./src/log.c
        ./src/signal.c
        ./src/strlcpy.c
        ./src/win32select.c
    )

    # 宏定义
    SET(LIBRARY_LIBEVENT_COMPILE_DEFINITIONS
        -D_WIN32_WINNT=0x0601
        -DLIBRARY_LIBEVENT_VALUE_JUST_FOR_TEST=818
    )

    # 编译选项
    SET(LIBRARY_LIBEVENT_COMPILE_OPTIONS
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
    SET(LIBRARY_LIBEVENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/3rd/libevent
        ${PROJECT_SOURCE_DIR}/3rd/libevent/include
        ${PROJECT_SOURCE_DIR}/3rd/libevent/src
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-linux/include
    )

    # 源文件
    SET(LIBRARY_LIBEVENT_DIR_SRC_LIST
        ./src/buffer.c
        ./src/bufferevent.c
        ./src/bufferevent_filter.c
        ./src/bufferevent_openssl.c
        ./src/bufferevent_pair.c
        ./src/bufferevent_ratelim.c
        ./src/bufferevent_sock.c
        ./src/epoll.c
        ./src/evdns.c
        ./src/event.c
        ./src/event_tagging.c
        ./src/evmap.c
        ./src/evrpc.c
        ./src/evthread.c
        ./src/evthread_pthread.c
        ./src/evutil.c
        ./src/evutil_rand.c
        ./src/evutil_time.c
        ./src/http.c
        ./src/listener.c
        ./src/log.c
        ./src/poll.c
        ./src/select.c
        ./src/signal.c
        ./src/strlcpy.c
    )

    # 宏定义
    SET(LIBRARY_LIBEVENT_COMPILE_DEFINITIONS
        -DLIBRARY_LIBEVENT_VALUE_JUST_FOR_TEST=818
    )

    # 编译选项
    SET(LIBRARY_LIBEVENT_COMPILE_OPTIONS
        -Wno-format
        -Wno-unused-parameter
        -Wno-unused-value
    )
ENDIF()

MESSAGE(STATUS "---------------------------------------------------------------------")

# 生成静态库
ADD_LIBRARY(${LIBRARY_LIBEVENT_BUILD_TARGET_NAME}                STATIC  ${LIBRARY_LIBEVENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${LIBRARY_LIBEVENT_BUILD_TARGET_NAME} PRIVATE ${LIBRARY_LIBEVENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${LIBRARY_LIBEVENT_BUILD_TARGET_NAME} PRIVATE ${LIBRARY_LIBEVENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${LIBRARY_LIBEVENT_BUILD_TARGET_NAME}     PRIVATE ${LIBRARY_LIBEVENT_COMPILE_OPTIONS})

MESSAGE(STATUS "LIBRARY_LIBEVENT_DIR_SRC_LIST        = ${LIBRARY_LIBEVENT_DIR_SRC_LIST}")
MESSAGE(STATUS "LIBRARY_LIBEVENT_INCLUDE_DIR         = ${LIBRARY_LIBEVENT_INCLUDE_DIR}")
MESSAGE(STATUS "LIBRARY_LIBEVENT_COMPILE_DEFINITIONS = ${LIBRARY_LIBEVENT_COMPILE_DEFINITIONS}")
MESSAGE(STATUS "LIBRARY_LIBEVENT_COMPILE_OPTIONS     = ${LIBRARY_LIBEVENT_COMPILE_OPTIONS}")