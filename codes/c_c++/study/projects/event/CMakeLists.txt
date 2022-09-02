# 生成目标名字
SET(EXEC_EVENT_BUILD_TARGET_NAME "event")

# 头文件目录、依赖库目录、依赖库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(EXEC_EVENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/projects/event
        ${PROJECT_SOURCE_DIR}/3rd/libevent/include
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-windows-static/include
    )

    # 依赖库目录
    SET(EXEC_EVENT_LINK_LIBS_DIR
        ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-windows-static/${VCPKG_LIBS_DIR_REFIX}/lib
    )

    # 依赖库
    SET(EXEC_EVENT_LINK_LIBS
        libevent-all
        libssl
        libcrypto
        ws2_32
        crypt32
        iphlpapi
    )

    # 宏定义
    SET(EXEC_EVENT_COMPILE_DEFINITIONS
        -D_WIN32_WINNT=0x0601
        -DEXEC_EVENT_VALUE_JUST_FOR_TEST=818
    )

    # 编译选项
    SET(EXEC_EVENT_COMPILE_OPTIONS
        # ...
    )
ELSE()
    # 头文件目录
    SET(EXEC_EVENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/projects/event
        ${PROJECT_SOURCE_DIR}/3rd/libevent/include
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-linux/include
    )

    # 依赖库目录
    SET(EXEC_EVENT_LINK_LIBS_DIR
        ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-linux/${VCPKG_LIBS_DIR_REFIX}/lib
    )

    # 依赖库
    SET(EXEC_EVENT_LINK_LIBS
        -levent-all
        -lssl
        -lcrypto
        -lpthread
        -ldl
    )

    # 宏定义
    SET(EXEC_EVENT_COMPILE_DEFINITIONS
        -DEXEC_EVENT_VALUE_JUST_FOR_TEST=818
    )

    # 编译选项
    SET(EXEC_EVENT_COMPILE_OPTIONS
        # ...
    )
ENDIF()

MESSAGE(STATUS "---------------------------------------------------------------------")

# 递归添加[./]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(./ EXEC_EVENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE EXEC_EVENT_DIR_SRC_LIST ./*.c ./*.cc ./*.cpp)

# 生成目标可执行文件
ADD_EXECUTABLE(${EXEC_EVENT_BUILD_TARGET_NAME}                     ${EXEC_EVENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${EXEC_EVENT_BUILD_TARGET_NAME} PRIVATE ${EXEC_EVENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${EXEC_EVENT_BUILD_TARGET_NAME} PRIVATE ${EXEC_EVENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${EXEC_EVENT_BUILD_TARGET_NAME}     PRIVATE ${EXEC_EVENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${EXEC_EVENT_BUILD_TARGET_NAME}    PRIVATE ${EXEC_EVENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${EXEC_EVENT_BUILD_TARGET_NAME}      PRIVATE ${EXEC_EVENT_LINK_LIBS})

MESSAGE(STATUS "EXEC_EVENT_DIR_SRC_LIST        = ${EXEC_EVENT_DIR_SRC_LIST}")
MESSAGE(STATUS "EXEC_EVENT_INCLUDE_DIR         = ${EXEC_EVENT_INCLUDE_DIR}")
MESSAGE(STATUS "EXEC_EVENT_COMPILE_DEFINITIONS = ${EXEC_EVENT_COMPILE_DEFINITIONS}")
MESSAGE(STATUS "EXEC_EVENT_COMPILE_OPTIONS     = ${EXEC_EVENT_COMPILE_OPTIONS}")
MESSAGE(STATUS "EXEC_EVENT_LINK_LIBS_DIR       = ${EXEC_EVENT_LINK_LIBS_DIR}")
MESSAGE(STATUS "EXEC_EVENT_LINK_LIBS           = ${EXEC_EVENT_LINK_LIBS}")