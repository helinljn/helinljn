# 项目名字
PROJECT(lua)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DMICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS=0
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4244
        /wd4310
        /wd4324
        /wd4702
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DLUA_USE_LINUX
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ENDIF()

# 源文件
SET(CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lapi.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lauxlib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lbaselib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lbitlib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lcode.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lcorolib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lctype.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/ldblib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/ldebug.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/ldo.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/ldump.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lfunc.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lgc.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/linit.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/liolib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/llex.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lmathlib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lmem.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/loadlib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lobject.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lopcodes.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/loslib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lparser.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lstate.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lstring.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lstrlib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/ltable.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/ltablib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/ltests.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/ltm.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lundump.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lutf8lib.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lvm.c
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/lzio.c
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})