# 项目名字
PROJECT(PocoRedis)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Net/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/include
)

# 宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DRedis_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4244
        /wd4458
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        PocoNet
        PocoFoundation
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DRedis_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-sign-compare
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        PocoNet
        PocoFoundation
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/include/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/include/*.hpp
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/src/*.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/src/*.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/src/*.cpp
)

# 生成动态库
ADD_LIBRARY(${PROJECT_NAME}                SHARED  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
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
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Redis/src PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ENDIF()