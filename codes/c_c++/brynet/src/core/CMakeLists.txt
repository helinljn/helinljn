# 项目名字
PROJECT(core)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_3RD_DIR_OPENSSL}/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/brynet/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include
)

# 宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DCORE_LIB_EXPORT
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        -DBRYNET_USE_OPENSSL
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        quill
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
        -DBRYNET_USE_OPENSSL
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        quill
        ssl
        crypto
        pthread
        dl
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${PROJECT_SOURCE_DIR}/*.h
    ${PROJECT_SOURCE_DIR}/*.hpp
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${PROJECT_SOURCE_DIR}/*.c
    ${PROJECT_SOURCE_DIR}/*.cc
    ${PROJECT_SOURCE_DIR}/*.cpp
)

# brynet源文件
FILE(GLOB_RECURSE BRYNET_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/brynet/include/brynet/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/brynet/include/brynet/*.hpp
)

# 生成动态库
ADD_LIBRARY(${PROJECT_NAME}                SHARED  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST} ${BRYNET_SRC_LIST})
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

    SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/brynet/include/brynet PREFIX "brynet"
        FILES ${BRYNET_SRC_LIST})

    # Windows平台下的OpenSSL使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
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
    # Linux平台下的OpenSSL使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
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