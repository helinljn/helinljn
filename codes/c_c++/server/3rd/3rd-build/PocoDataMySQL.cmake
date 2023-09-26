# 项目名字
PROJECT(PocoDataMySQL)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include
    ${CMAKE_3RD_DIR_MYSQL}/include/mysql
)

# 宏定义、编译选项
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4245
        /wd4706
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-sign-compare
        -Wno-deprecated-copy
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include/*.hpp
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/src/*.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/src/*.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/src/*.cpp
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PUBLIC  PocoData PocoFoundation)

# VS工程设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/src PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})

    # Windows平台下的MySQL和OpenSSL使用的都是动态库，所以需要将其对应的动态库拷贝至生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_MYSQL}/lib/libmysql.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_PROJECT_ROOT_DIR}/tools/openssl/libssl-1_1-x64.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_PROJECT_ROOT_DIR}/tools/openssl/libcrypto-1_1-x64.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
    )
ELSE()
    # Linux平台下的MySQL和OpenSSL使用的都是动态库，所以需要将其对应的动态库拷贝至生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_MYSQL}/lib/libmysqlclient.so.21
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_OPENSSL}/lib/libssl.so.1.1
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_OPENSSL}/lib/libcrypto.so.1.1
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND cd ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE} && ln -sf libmysqlclient.so.21 libmysqlclient.so
        COMMAND cd ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE} && ln -sf libssl.so.1.1        libssl.so
        COMMAND cd ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE} && ln -sf libcrypto.so.1.1     libcrypto.so
    )

    # Linux平台下rpath动态库运行路径修改，优先查找当前目录下的动态库
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND patchelf --set-rpath ./
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/libmysqlclient.so
        COMMAND patchelf --set-rpath ./
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/libssl.so
        COMMAND patchelf --set-rpath ./
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/libcrypto.so
    )
ENDIF()