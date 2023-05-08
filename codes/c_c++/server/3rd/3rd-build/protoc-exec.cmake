# 项目名字
PROJECT(protoc-exec)

# 头文件目录、链接库目录、生成依赖库、链接库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
    )

    # 链接库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
    )

    # 生成依赖库
    SET(CURRENT_DEPENDENT_LIBS
        protoc
        protobuf
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        ${CURRENT_DEPENDENT_LIBS}
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
    )

    # 链接库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
    )

    # 生成依赖库
    SET(CURRENT_DEPENDENT_LIBS
        protoc
        protobuf
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        ${CURRENT_DEPENDENT_LIBS}
        pthread
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-unused-parameter
    )
ENDIF()

# 源文件
SET(CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/main.cc
)

# 生成目标可执行文件
ADD_EXECUTABLE(${PROJECT_NAME}                     ${CURRENT_DIR_SRC_LIST})
ADD_DEPENDENCIES(${PROJECT_NAME}                   ${CURRENT_DEPENDENT_LIBS})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PRIVATE ${CURRENT_LINK_LIBS})