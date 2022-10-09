# 项目名字
PROJECT(test)

# 头文件目录、依赖库目录、依赖库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/../protocol
        ${PROJECT_SOURCE_DIR}/../../3rd/asio/include
        ${PROJECT_SOURCE_DIR}/../../3rd/hiredis/include
        ${PROJECT_SOURCE_DIR}/../../3rd/http-parser/include
        ${PROJECT_SOURCE_DIR}/../../3rd/jsoncpp/include
        ${PROJECT_SOURCE_DIR}/../../3rd/libevent/include
        ${PROJECT_SOURCE_DIR}/../../3rd/lua/include
        ${PROJECT_SOURCE_DIR}/../../3rd/LuaBridge/include
        ${PROJECT_SOURCE_DIR}/../../3rd/protobuf/include
        ${PROJECT_SOURCE_DIR}/../../3rd/rapidjson/include
        ${PROJECT_SOURCE_DIR}/../../3rd/rapidxml/include
        ${PROJECT_SOURCE_DIR}/../../../fmt/fmt/include
        ${PROJECT_SOURCE_DIR}/../../../spdlog/spdlog/include
        ${PROJECT_SOURCE_DIR}/../../../hana/hana/include
        ${PROJECT_SOURCE_DIR}/../../../sqlpp11/sqlpp11/include
        ${PROJECT_SOURCE_DIR}/../../../vcpkg/vcpkg/packages/openssl_x64-windows-static/include
    )

    # 依赖库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${PROJECT_SOURCE_DIR}/../../build/${CMAKE_BUILD_TYPE}
        ${PROJECT_SOURCE_DIR}/../../../vcpkg/vcpkg/packages/openssl_x64-windows-static/${VCPKG_LIBS_DIR_REFIX}/lib
    )

    # 依赖库
    SET(CURRENT_LINK_LIBS
        libprotocol
        libasio
        libhiredis
        libhttp-parser
        libjsoncpp
        libevent
        liblua
        libprotobuf
        libssl
        libcrypto
        ws2_32
        crypt32
        iphlpapi
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DASIO_STANDALONE
        -DASIO_SEPARATE_COMPILATION
        -DFMT_HEADER_ONLY
        -DSPDLOG_FMT_EXTERNAL
        -DLUABRIDGE_CXX17
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4127
        /wd4819
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/../protocol
        ${PROJECT_SOURCE_DIR}/../../3rd/asio/include
        ${PROJECT_SOURCE_DIR}/../../3rd/hiredis/include
        ${PROJECT_SOURCE_DIR}/../../3rd/http-parser/include
        ${PROJECT_SOURCE_DIR}/../../3rd/jsoncpp/include
        ${PROJECT_SOURCE_DIR}/../../3rd/libevent/include
        ${PROJECT_SOURCE_DIR}/../../3rd/lua/include
        ${PROJECT_SOURCE_DIR}/../../3rd/LuaBridge/include
        ${PROJECT_SOURCE_DIR}/../../3rd/protobuf/include
        ${PROJECT_SOURCE_DIR}/../../3rd/rapidjson/include
        ${PROJECT_SOURCE_DIR}/../../3rd/rapidxml/include
        ${PROJECT_SOURCE_DIR}/../../../fmt/fmt/include
        ${PROJECT_SOURCE_DIR}/../../../spdlog/spdlog/include
        ${PROJECT_SOURCE_DIR}/../../../hana/hana/include
        ${PROJECT_SOURCE_DIR}/../../../sqlpp11/sqlpp11/include
        ${PROJECT_SOURCE_DIR}/../../../vcpkg/vcpkg/packages/openssl_x64-linux/include
    )

    # 依赖库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${PROJECT_SOURCE_DIR}/../../build/${CMAKE_BUILD_TYPE}
        ${PROJECT_SOURCE_DIR}/../../../vcpkg/vcpkg/packages/openssl_x64-linux/${VCPKG_LIBS_DIR_REFIX}/lib
    )

    # 依赖库
    SET(CURRENT_LINK_LIBS
        -lprotocol
        -lasio
        -lhiredis
        -lhttp-parser
        -ljsoncpp
        -levent
        -llua
        -lprotobuf
        -lssl
        -lcrypto
        -lpthread
        -lm
        -ldl
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DASIO_STANDALONE
        -DASIO_SEPARATE_COMPILATION
        -DFMT_HEADER_ONLY
        -DSPDLOG_FMT_EXTERNAL
        -DLUABRIDGE_CXX17
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-type-limits
    )
ENDIF()

# 递归添加[./]目录下所有源文件(不包括头文件)
#AUX_SOURCE_DIRECTORY(./ CURRENT_DIR_SRC_LIST)
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST ./*.c ./*.cc ./*.cpp)

# 生成目标可执行文件
ADD_EXECUTABLE(${PROJECT_NAME}                     ${CURRENT_DIR_SRC_LIST})
ADD_DEPENDENCIES(${PROJECT_NAME}                   ${CURRENT_DEPENDENT_LIBS_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CURRENT_LINK_LIBS_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PRIVATE ${CURRENT_LINK_LIBS})

# 调试信息
MESSAGE(STATUS "---------------------------------------------------------------------")
MESSAGE(STATUS "CMAKE_PROJECT_NAME          = ${CMAKE_PROJECT_NAME}")
MESSAGE(STATUS "PROJECT_NAME                = ${PROJECT_NAME}")
MESSAGE(STATUS "PROJECT_SOURCE_DIR          = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR          = ${PROJECT_BINARY_DIR}")
MESSAGE(STATUS "CURRENT_DIR_SRC_LIST        = ${CURRENT_DIR_SRC_LIST}")
MESSAGE(STATUS "CURRENT_INCLUDE_DIR         = ${CURRENT_INCLUDE_DIR}")
MESSAGE(STATUS "CURRENT_COMPILE_DEFINITIONS = ${CURRENT_COMPILE_DEFINITIONS}")
MESSAGE(STATUS "CURRENT_COMPILE_OPTIONS     = ${CURRENT_COMPILE_OPTIONS}")
MESSAGE(STATUS "CURRENT_LINK_LIBS_DIR       = ${CURRENT_LINK_LIBS_DIR}")
MESSAGE(STATUS "CURRENT_LINK_LIBS           = ${CURRENT_LINK_LIBS}")