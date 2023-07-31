# 项目名字
PROJECT(event)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/compat
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/3rd-build/libevent/include
    ${CMAKE_3RD_DIR_OPENSSL}/include
)

# 源文件、宏定义、编译选项
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 源文件
    SET(CURRENT_DIR_SRC_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/buffer.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/buffer_iocp.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_async.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_filter.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_openssl.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_pair.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_ratelim.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_sock.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evdns.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/event.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/event_iocp.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/event_tagging.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evmap.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evrpc.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evthread.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evthread_win32.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evutil.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evutil_rand.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evutil_time.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/http.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/listener.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/log.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/signal.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/strlcpy.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/win32select.c
    )

    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DMICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS=0
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4244
        /wd4267
        /wd4996
    )
ELSE()
    # 源文件
    SET(CURRENT_DIR_SRC_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/buffer.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_filter.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_openssl.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_pair.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_ratelim.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/bufferevent_sock.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/epoll.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evdns.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/event.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/event_tagging.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evmap.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evrpc.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evthread.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evthread_pthread.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evutil.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evutil_rand.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/evutil_time.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/http.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/listener.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/log.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/poll.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/select.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/signal.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/strlcpy.c
    )

    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-unused-value
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include/*.hpp
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

# VS工程设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ENDIF()