# 生成目标名字
SET(EXEC_ASIO_BUILD_TARGET_NAME "asio")

# 头文件目录、依赖库目录、依赖库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(EXEC_ASIO_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/projects/asio
        ${PROJECT_SOURCE_DIR}/../asio/asio/asio/include
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-windows-static/include
    )

    # 依赖库目录
    SET(EXEC_ASIO_LINK_LIBS_DIR
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-windows-static/${VCPKG_LIBS_DIR_REFIX}/lib
    )

    # 依赖库
    SET(EXEC_ASIO_LINK_LIBS
        libssl
        libcrypto
        ws2_32
        crypt32
        iphlpapi
    )

    # 宏定义
    SET(EXEC_ASIO_COMPILE_DEFINITIONS
        -D_WIN32_WINNT=0x0601
        -DEXEC_ASIO_VALUE_JUST_FOR_TEST=818
        -DASIO_STANDALONE
    )

    # 编译选项
    SET(EXEC_ASIO_COMPILE_OPTIONS
        # ...
    )
ELSE()
    # 头文件目录
    SET(EXEC_ASIO_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/projects/asio
        ${PROJECT_SOURCE_DIR}/../asio/asio/asio/include
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-linux/include
    )

    # 依赖库目录
    SET(EXEC_ASIO_LINK_LIBS_DIR
        ${PROJECT_SOURCE_DIR}/../vcpkg/vcpkg/packages/openssl_x64-linux/${VCPKG_LIBS_DIR_REFIX}/lib
    )

    # 依赖库
    SET(EXEC_ASIO_LINK_LIBS
        -lssl
        -lcrypto
        -lpthread
        -ldl
    )

    # 宏定义
    SET(EXEC_ASIO_COMPILE_DEFINITIONS
        -DEXEC_ASIO_VALUE_JUST_FOR_TEST=818
        -DASIO_STANDALONE
    )

    # 编译选项
    SET(EXEC_ASIO_COMPILE_OPTIONS
        # ...
    )
ENDIF()

MESSAGE(STATUS "---------------------------------------------------------------------")

# 递归添加[./]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(./ EXEC_ASIO_DIR_SRC_LIST)
FILE(GLOB_RECURSE EXEC_ASIO_DIR_SRC_LIST ./*.c ./*.cc ./*.cpp)

# 生成目标可执行文件
ADD_EXECUTABLE(${EXEC_ASIO_BUILD_TARGET_NAME}                     ${EXEC_ASIO_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${EXEC_ASIO_BUILD_TARGET_NAME} PRIVATE ${EXEC_ASIO_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${EXEC_ASIO_BUILD_TARGET_NAME} PRIVATE ${EXEC_ASIO_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${EXEC_ASIO_BUILD_TARGET_NAME}     PRIVATE ${EXEC_ASIO_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${EXEC_ASIO_BUILD_TARGET_NAME}    PRIVATE ${EXEC_ASIO_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${EXEC_ASIO_BUILD_TARGET_NAME}      PRIVATE ${EXEC_ASIO_LINK_LIBS})

MESSAGE(STATUS "EXEC_ASIO_DIR_SRC_LIST        = ${EXEC_ASIO_DIR_SRC_LIST}")
MESSAGE(STATUS "EXEC_ASIO_INCLUDE_DIR         = ${EXEC_ASIO_INCLUDE_DIR}")
MESSAGE(STATUS "EXEC_ASIO_COMPILE_DEFINITIONS = ${EXEC_ASIO_COMPILE_DEFINITIONS}")
MESSAGE(STATUS "EXEC_ASIO_COMPILE_OPTIONS     = ${EXEC_ASIO_COMPILE_OPTIONS}")
MESSAGE(STATUS "EXEC_ASIO_LINK_LIBS_DIR       = ${EXEC_ASIO_LINK_LIBS_DIR}")
MESSAGE(STATUS "EXEC_ASIO_LINK_LIBS           = ${EXEC_ASIO_LINK_LIBS}")