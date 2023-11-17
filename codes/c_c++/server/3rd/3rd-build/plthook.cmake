# 项目名字
PROJECT(plthook)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook
)

# 创建资源文件
IF(NOT EXISTS ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook/plthook.def)
    FILE(WRITE ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook/plthook.def
        "EXPORTS\n"
        "    plthook_open\n"
        "    plthook_open_by_handle\n"
        "    plthook_open_by_address\n"
        "    plthook_enum\n"
        "    plthook_replace\n"
        "    plthook_close\n"
        "    plthook_error\n"
    )
ENDIF()

# 头文件、源文件、资源文件、宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 头文件
    SET(CURRENT_DIR_INCLUDE_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook/plthook.h
    )

    # 源文件
    SET(CURRENT_DIR_SRC_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook/plthook_win32.c
    )

    # 资源文件
    SET(CURRENT_DIR_RES_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook/plthook.def
    )

    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4267
        /wd4996
        /wd5105
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        dbghelp
    )
ELSE()
    # 头文件
    SET(CURRENT_DIR_INCLUDE_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook/plthook.h
    )

    # 源文件
    SET(CURRENT_DIR_SRC_LIST
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook/plthook_elf.c
    )

    # 资源文件
    SET(CURRENT_DIR_RES_LIST
        # ...
    )

    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        dl
    )
ENDIF()

# 生成动态库
ADD_LIBRARY(${PROJECT_NAME}                SHARED  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST} ${CURRENT_DIR_RES_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CMAKE_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PUBLIC  ${CURRENT_LINK_LIBS})

# 其它设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # MSVC运行库设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # VS工程设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/plthook PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ENDIF()