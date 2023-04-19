# 项目名字
PROJECT(test)

# 头文件目录、链接库目录、生成依赖库、链接库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        #${CMAKE_PROJECT_ROOT_DIR}/3rd/asio/asio/include
        #${CMAKE_PROJECT_ROOT_DIR}/3rd/CppCommon/include
        #${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/googletest/googletest/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/jsoncpp/include/json
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/compat
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/projects/modules/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua
        ${CMAKE_PROJECT_ROOT_DIR}/projects/modules/lua/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/LuaBridge/Source
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
        ${CMAKE_PROJECT_ROOT_DIR}/projects/protocol
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/tinyxml2
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-windows/include
    )

    # 链接库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-windows/lib
    )

    # 生成依赖库
    SET(CURRENT_DEPENDENT_LIBS
        #asio
        #cppcommon
        #cppserver
        fmt
        gtest
        hiredis
        jsoncpp
        event
        lua
        protobuf
        protocol
        spdlog
        tinyxml2
        zlib
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        ${CURRENT_DEPENDENT_LIBS}
        libssl
        libcrypto
        crypt32
        dbghelp
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DSPDLOG_FMT_EXTERNAL
        -DSPDLOG_COMPILED_LIB
        -DLUABRIDGE_CXX17
        -DGTEST_DONT_DEFINE_TEST
        -DGTEST_DONT_DEFINE_FAIL
        -DGTEST_DONT_DEFINE_SUCCEED
        -DGTEST_DONT_DEFINE_ASSERT_TRUE
        -DGTEST_DONT_DEFINE_ASSERT_FALSE
        -DGTEST_DONT_DEFINE_ASSERT_EQ
        -DGTEST_DONT_DEFINE_ASSERT_NE
        -DGTEST_DONT_DEFINE_ASSERT_LE
        -DGTEST_DONT_DEFINE_ASSERT_LT
        -DGTEST_DONT_DEFINE_ASSERT_GE
        -DGTEST_DONT_DEFINE_ASSERT_GT
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4819
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        #${CMAKE_PROJECT_ROOT_DIR}/3rd/asio/asio/include
        #${CMAKE_PROJECT_ROOT_DIR}/3rd/CppCommon/include
        #${CMAKE_PROJECT_ROOT_DIR}/3rd/CppServer/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/googletest/googletest/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/jsoncpp/include/json
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/compat
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/projects/modules/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua
        ${CMAKE_PROJECT_ROOT_DIR}/projects/modules/lua/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/LuaBridge/Source
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
        ${CMAKE_PROJECT_ROOT_DIR}/projects/protocol
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/spdlog/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/tinyxml2
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/zlib
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-ubuntu-20.04/include
    )

    # 链接库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-ubuntu-20.04/lib
    )

    # 生成依赖库
    SET(CURRENT_DEPENDENT_LIBS
        #asio
        #cppcommon
        #cppserver
        fmt
        gtest
        hiredis
        jsoncpp
        event
        lua
        protobuf
        protocol
        spdlog
        tinyxml2
        zlib
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        ${CURRENT_DEPENDENT_LIBS}
        ssl
        crypto
        pthread
        dl
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DSPDLOG_FMT_EXTERNAL
        -DSPDLOG_COMPILED_LIB
        -DLUABRIDGE_CXX17
        -DGTEST_DONT_DEFINE_TEST
        -DGTEST_DONT_DEFINE_FAIL
        -DGTEST_DONT_DEFINE_SUCCEED
        -DGTEST_DONT_DEFINE_ASSERT_TRUE
        -DGTEST_DONT_DEFINE_ASSERT_FALSE
        -DGTEST_DONT_DEFINE_ASSERT_EQ
        -DGTEST_DONT_DEFINE_ASSERT_NE
        -DGTEST_DONT_DEFINE_ASSERT_LE
        -DGTEST_DONT_DEFINE_ASSERT_LT
        -DGTEST_DONT_DEFINE_ASSERT_GE
        -DGTEST_DONT_DEFINE_ASSERT_GT
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        # ...
    )
ENDIF()

# 递归添加[./]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(./ CURRENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ./*.c
    ./*.cc
    ./*.cpp
)

# 生成目标可执行文件
ADD_EXECUTABLE(${PROJECT_NAME}                     ${CURRENT_DIR_SRC_LIST})
ADD_DEPENDENCIES(${PROJECT_NAME}                   ${CURRENT_DEPENDENT_LIBS})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PRIVATE ${CURRENT_LINK_LIBS})