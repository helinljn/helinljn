# 项目名字
SET(CURRENT_TARGET_NAME mimalloc)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src
)

# 宏定义、编译选项、链接库
IF(WIN32)
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DMI_SHARED_LIB
        -DMI_SHARED_LIB_EXPORT
        -DMI_MALLOC_OVERRIDE
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4702
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        # ...
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DMI_SHARED_LIB
        -DMI_SHARED_LIB_EXPORT
        -DMI_MALLOC_OVERRIDE
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-pedantic
        -fno-builtin-malloc
        -fvisibility=hidden
        -ftls-model=initial-exec
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        # ...
    )
ENDIF()

# mimalloc头文件
SET(MIMALLOC_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/include/mimalloc.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/include/mimalloc-override.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/include/mimalloc-new-delete.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/include/mimalloc-stats.h
)

# mimalloc源文件
SET(MIMALLOC_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/alloc.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/alloc-aligned.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/alloc-posix.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/arena.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/bitmap.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/heap.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/init.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/libc.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/options.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/os.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/page.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/random.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/segment.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/segment-map.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/stats.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/src/prim/prim.c
)

# 生成动态库
ADD_LIBRARY(${CURRENT_TARGET_NAME} SHARED ${MIMALLOC_INCLUDE_LIST} ${MIMALLOC_SRC_LIST})
PROJECT_TARGET_APPLY_COMMON_OPTIONS(${CURRENT_TARGET_NAME})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_TARGET_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${CURRENT_TARGET_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${CURRENT_TARGET_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${CURRENT_TARGET_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${CURRENT_TARGET_NAME}    PRIVATE ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE})
TARGET_LINK_LIBRARIES(${CURRENT_TARGET_NAME}      PUBLIC  ${CURRENT_LINK_LIBS})

# 其它设置
IF(WIN32)
    # mimalloc 在 MSVC 下按 C++ 编译，以使用较新的原子实现
    SET_SOURCE_FILES_PROPERTIES(${MIMALLOC_SRC_LIST} PROPERTIES LANGUAGE CXX)

    # Windows 下使用 mimalloc 官方 redirect 机制
    ADD_CUSTOM_COMMAND(TARGET ${CURRENT_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/bin/mimalloc-redirect.dll"
            "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/mimalloc-redirect.dll"
    )

    TARGET_LINK_LIBRARIES(${CURRENT_TARGET_NAME} PRIVATE "${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/bin/mimalloc-redirect.lib")

    # MSVC运行库设置
    SET_PROPERTY(TARGET ${CURRENT_TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # VS工程设置
    SET_PROPERTY(TARGET ${CURRENT_TARGET_NAME} PROPERTY FOLDER "projects")
    SET_PROPERTY(TARGET ${CURRENT_TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DEBUGGER_WORKING_DIRECTORY}")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc PREFIX "mimalloc"
        FILES ${MIMALLOC_INCLUDE_LIST} ${MIMALLOC_SRC_LIST})
ENDIF()