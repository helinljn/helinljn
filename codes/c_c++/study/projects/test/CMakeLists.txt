# 项目名字
PROJECT(test)

# 头文件目录、依赖库目录、依赖库、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_PROJECT_ROOT_DIR}/projects/protocol
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/http-parser/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/jsoncpp/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/LuaBridge/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/tinyxml2/include
        ${CMAKE_PROJECT_ROOT_DIR}/../asio/asio/asio/include
        ${CMAKE_PROJECT_ROOT_DIR}/../date/date/include
        ${CMAKE_PROJECT_ROOT_DIR}/../spdlog/spdlog/include
        ${CMAKE_PROJECT_ROOT_DIR}/../sqlpp11/sqlpp11/include
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-windows/include
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/mysqlclient-8.0.30/x64-windows/include/mysql
    )

    # 依赖库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-windows/lib
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/mysqlclient-8.0.30/x64-windows/lib
    )

    # 依赖库
    SET(CURRENT_LINK_LIBS
        libprotocol
        libhiredis
        libhttp-parser
        libjsoncpp
        libevent
        liblua
        libprotobuf
        libtinyxml2
        libssl
        libcrypto
        mysqlclient
        ws2_32
        crypt32
        iphlpapi
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DLUABRIDGE_CXX17
        -DASIO_STANDALONE
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4018
        /wd4127
        /wd4267
        /wd4819
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_PROJECT_ROOT_DIR}/projects/protocol
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/hiredis/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/http-parser/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/jsoncpp/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/libevent/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/lua/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/LuaBridge/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/include
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/tinyxml2/include
        ${CMAKE_PROJECT_ROOT_DIR}/../asio/asio/asio/include
        ${CMAKE_PROJECT_ROOT_DIR}/../date/date/include
        ${CMAKE_PROJECT_ROOT_DIR}/../spdlog/spdlog/include
        ${CMAKE_PROJECT_ROOT_DIR}/../sqlpp11/sqlpp11/include
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-ubuntu-20.04/include
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/mysqlclient-8.0.30/x64-ubuntu-20.04/include/mysql
    )

    # 依赖库目录
    SET(CURRENT_LINK_LIBS_DIR
        ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE}
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-1.1.1n/x64-ubuntu-20.04/lib
        ${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/mysqlclient-8.0.30/x64-ubuntu-20.04/lib
    )

    # 依赖库
    SET(CURRENT_LINK_LIBS
        -lprotocol
        -lhiredis
        -lhttp-parser
        -ljsoncpp
        -levent
        -llua
        -lprotobuf
        -ltinyxml2
        -lssl
        -lcrypto
        -lmysqlclient
        -lpthread
        -lm
        -ldl
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DLUABRIDGE_CXX17
        -DASIO_STANDALONE
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-sign-compare
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