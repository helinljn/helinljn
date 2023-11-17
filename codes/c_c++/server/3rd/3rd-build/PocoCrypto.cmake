# 项目名字
PROJECT(PocoCrypto)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_3RD_DIR_OPENSSL}/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/include
)

# 链接库目录
SET(CURRENT_LINK_LIBS_DIR
    ${CMAKE_3RD_DIR_MYSQL}/lib
    ${CMAKE_3RD_DIR_OPENSSL}/lib
    ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
)

# 宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DCrypto_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4267
        /wd4456
        /wd4702
        /wd4706
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        PocoFoundation
        ssl
        crypto
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DCrypto_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-sign-compare
        -Wno-unused-parameter
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        PocoFoundation
        ssl
        crypto
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/include/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/include/*.hpp
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/src/*.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/src/*.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/src/*.cpp
)

# 生成动态库
ADD_LIBRARY(${PROJECT_NAME}                SHARED  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PUBLIC  ${CURRENT_LINK_LIBS})

# 其它设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # MSVC运行库设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # VS工程设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Crypto/src PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})

    # Windows平台下的OpenSSL使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_OPENSSL}/lib/ssl.lib
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_OPENSSL}/lib/crypto.lib
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_PROJECT_ROOT_DIR}/tools/openssl/libssl-1_1-x64.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_PROJECT_ROOT_DIR}/tools/openssl/libcrypto-1_1-x64.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
    )
ELSE()
    # Linux平台下的OpenSSL使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_OPENSSL}/lib/libssl.so.1.1
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_OPENSSL}/lib/libcrypto.so.1.1
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libssl.so.1.1 libssl.so
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
            ln -sf libcrypto.so.1.1 libcrypto.so
    )
ENDIF()