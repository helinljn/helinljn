# 项目名字
PROJECT(core)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_3RD_DIR_CPPTRACE}/include
    ${CMAKE_3RD_DIR_OPENSSL}/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/brynet/include
)

# 宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DCORE_LIB_EXPORT
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        -DFMT_HEADER_ONLY
        -DSPDLOG_FMT_EXTERNAL
        -DBRYNET_USE_OPENSSL
        -DDOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        cpptrace
        libssl
        libcrypto
        ws2_32
        dbghelp
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DCORE_LIB_EXPORT
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        -DFMT_HEADER_ONLY
        -DSPDLOG_FMT_EXTERNAL
        -DBRYNET_USE_OPENSSL
        -DDOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        cpptrace
        ssl
        crypto
        pthread
        dl
    )
ENDIF()

# core源文件
FILE(GLOB_RECURSE CORE_SRC_LIST
    ${PROJECT_SOURCE_DIR}/*.h
    ${PROJECT_SOURCE_DIR}/*.inl
    ${PROJECT_SOURCE_DIR}/*.hpp
    ${PROJECT_SOURCE_DIR}/*.c
    ${PROJECT_SOURCE_DIR}/*.cc
    ${PROJECT_SOURCE_DIR}/*.cpp
)

# fmt源文件
FILE(GLOB_RECURSE FMT_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include/fmt/*.h
)

# spdlog源文件
FILE(GLOB_RECURSE SPDLOG_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include/spdlog/*.h
)

# brynet源文件
FILE(GLOB_RECURSE BRYNET_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/brynet/include/brynet/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/brynet/include/brynet/*.hpp
)

# doctest源文件
FILE(GLOB_RECURSE DOCTEST_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/doctest/doctest/doctest.h
)

# 生成动态库
ADD_LIBRARY(${PROJECT_NAME}                SHARED  ${CORE_SRC_LIST} ${FMT_SRC_LIST} ${SPDLOG_SRC_LIST} ${BRYNET_SRC_LIST} ${DOCTEST_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PUBLIC  ${CURRENT_LINK_LIBS})

# 其它设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # MSVC运行库设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # VS工程设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "projects")

    SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} PREFIX "core"
        FILES ${CORE_SRC_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include/fmt PREFIX "fmt"
        FILES ${FMT_SRC_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include/spdlog PREFIX "spdlog"
        FILES ${SPDLOG_SRC_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/brynet/include/brynet PREFIX "brynet"
        FILES ${BRYNET_SRC_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/doctest/doctest PREFIX "doctest"
        FILES ${DOCTEST_SRC_LIST})

    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
        # Windows平台下的cpptrace使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_CPPTRACE}/lib/cpptrace.lib
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_CPPTRACE}/lib/cpptrace.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        # Windows平台下的OpenSSL使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_OPENSSL}/lib/libssl.lib
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_OPENSSL}/lib/libcrypto.lib
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_PROJECT_ROOT_DIR}/tools/openssl/libssl-1_1-x64.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_PROJECT_ROOT_DIR}/tools/openssl/libcrypto-1_1-x64.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
    )
ELSE()
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
        # Linux平台下的cpptrace使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_CPPTRACE}/lib/libcpptrace.so.1.0.4
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_CPPTRACE}/lib/libdwarf.so.2.3.1
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_CPPTRACE}/lib/libz.so.1.3.1
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_CPPTRACE}/lib/libzstd.so.1.5.7
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libcpptrace.so.1.0.4 libcpptrace.so.1
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libcpptrace.so.1 libcpptrace.so
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libdwarf.so.2.3.1 libdwarf.so.2
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libdwarf.so.2 libdwarf.so
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libz.so.1.3.1 libz.so.1
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libz.so.1 libz.so
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libzstd.so.1.5.7 libzstd.so.1
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libzstd.so.1 libzstd.so
        # Linux平台下的OpenSSL使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_OPENSSL}/lib/libssl.so.1.1
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_3RD_DIR_OPENSSL}/lib/libcrypto.so.1.1
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libssl.so.1.1 libssl.so
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libcrypto.so.1.1 libcrypto.so
    )
ENDIF()