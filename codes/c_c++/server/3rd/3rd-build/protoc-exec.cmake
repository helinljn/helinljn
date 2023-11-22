# 项目名字
PROJECT(protoc-exec)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
)

# 宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        protoc
        protobuf
    )
ELSE()
    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-unused-parameter
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        protoc
        protobuf
        pthread
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/*.hpp
)

# 源文件
SET(CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/main.cc
)

# 生成可执行文件
ADD_EXECUTABLE(${PROJECT_NAME}                     ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PRIVATE ${CURRENT_LINK_LIBS})

# 其它设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # MSVC运行库设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # 导出所有符号设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS True)
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS True)

    # VS工程设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})

    # 将当前可执行文件拷贝至protobuf生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.exe
            ${CMAKE_PROJECT_ROOT_DIR}/tools/protoc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/libprotoc.dll
            ${CMAKE_PROJECT_ROOT_DIR}/tools/protoc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/libprotobuf.dll
            ${CMAKE_PROJECT_ROOT_DIR}/tools/protoc
    )
ELSE()
    # 将当前可执行文件拷贝至protobuf生成目录
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}
            ${CMAKE_PROJECT_ROOT_DIR}/tools/protoc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/libprotoc.so
            ${CMAKE_PROJECT_ROOT_DIR}/tools/protoc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}/libprotobuf.so
            ${CMAKE_PROJECT_ROOT_DIR}/tools/protoc
    )
ENDIF()