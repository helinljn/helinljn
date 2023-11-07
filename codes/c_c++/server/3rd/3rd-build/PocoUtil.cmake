# 项目名字
PROJECT(PocoUtil)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Foundation/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/JSON/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/XML/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include
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
        -DUtil_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4456
        /wd4457
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        PocoXML
        PocoJSON
        PocoFoundation
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DUtil_EXPORTS
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
        PocoXML
        PocoJSON
        PocoFoundation
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include/*.hpp
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/*.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/*.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/*.cpp
)

# 排除特定平台实现源文件
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    GET_FILENAME_COMPONENT(
        WinRegistryConfiguration
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/WinRegistryConfiguration.cpp
        ABSOLUTE)
    GET_FILENAME_COMPONENT(
        WinRegistryKey
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/WinRegistryKey.cpp
        ABSOLUTE)
    GET_FILENAME_COMPONENT(
        WinService
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src/WinService.cpp
        ABSOLUTE)

    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${WinRegistryConfiguration})
    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${WinRegistryKey})
    LIST(REMOVE_ITEM CURRENT_DIR_SRC_LIST ${WinService})
ENDIF()

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

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/Util/src PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})

    # 单独为Windows下C++源文件设置额外的编译选项
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: /Zc:strictStrings->)
ELSE()
    # Linux平台下rpath动态库运行路径修改，优先查找当前目录下的动态库
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND patchelf --set-rpath ./
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/lib${PROJECT_NAME}.so
    )
ENDIF()