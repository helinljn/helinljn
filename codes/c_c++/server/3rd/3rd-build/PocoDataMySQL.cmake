# 项目名字
PROJECT(PocoDataMySQL)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_3RD_DIR_MYSQL}/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include
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
        -DMySQL_EXPORTS
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

    # 链接库
    SET(CURRENT_LINK_LIBS
        PocoData
        PocoFoundation
        libmysql
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DMySQL_EXPORTS
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

    # 链接库
    SET(CURRENT_LINK_LIBS
        PocoData
        PocoFoundation
        mysqlclient
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

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Data/MySQL/src PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})

    # Windows平台下的MySQL使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_MYSQL}/lib/libmysql.dll
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
    )
ELSE()
    # Linux平台下的MySQL使用的是动态库，所以需要将其对应的动态库拷贝至生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_3RD_DIR_MYSQL}/lib/libmysqlclient.so.21
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        COMMAND cd ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE} && ln -sf libmysqlclient.so.21 libmysqlclient.so
    )
ENDIF()