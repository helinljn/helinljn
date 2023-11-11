# 项目名字
PROJECT(funchook)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/distorm/include
)

# 链接库目录
SET(CURRENT_LINK_LIBS_DIR
    ${CMAKE_3RD_DIR_MYSQL}/lib
    ${CMAKE_3RD_DIR_OPENSSL}/lib
    ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
)

# 创建配置文件
IF(NOT EXISTS ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/config.h)
    FILE(WRITE ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/config.h
        "#define DISASM_DISTORM 1\n"
        "#define SIZEOF_VOID_P 8\n"
    )
ENDIF()

# 宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DFUNCHOOK_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4210
        /wd4244
        /wd4389
        /wd4456
        /wd4701
        /wd5105
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        distorm
        psapi
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DFUNCHOOK_EXPORTS
        -D_GNU_SOURCE
        -DGNU_SPECIFIC_STRERROR_R
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-format
        -Wno-pedantic
        -Wno-overflow
        -Wno-sign-compare
        -Wno-unused-parameter
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        distorm
        dl
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/include/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/include/*.hpp
)

# 源文件
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    ENABLE_LANGUAGE(ASM_MASM)
    SET(CURRENT_DIR_SRC_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/funchook.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/arch_x86.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/os_windows.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/disasm_distorm.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/prehook-x86_64-ms.asm
    )
ELSE()
    SET(CURRENT_DIR_SRC_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/funchook.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/arch_x86.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/os_unix.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/disasm_distorm.c
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src/prehook-x86_64-sysv.S
    )
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

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/funchook/src PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ELSE()
    # Linux平台下rpath动态库运行路径修改，优先查找当前目录下的动态库
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND patchelf --set-rpath ./
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/lib${PROJECT_NAME}.so
    )
ENDIF()