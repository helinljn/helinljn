IF(WIN32)
    SET(Poco_DIR              "${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/.build/windows/x64-${CMAKE_BUILD_TYPE}/Poco" CACHE PATH "Poco package directory" FORCE)
    SET(POCO_MYSQL_ROOT_DIR   "${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/mysql-8.0.45/x64-windows"                 CACHE PATH "Poco MySQL root" FORCE)
    SET(POCO_OPENSSL_ROOT_DIR "${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-3.0.20/x64-windows"               CACHE PATH "Poco OpenSSL root" FORCE)
ELSE()
    SET(Poco_DIR              "${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/.build/linux/x64-${CMAKE_BUILD_TYPE}/Poco"   CACHE PATH "Poco package directory" FORCE)
    SET(POCO_MYSQL_ROOT_DIR   "${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/mysql-8.0.45/x64-ubuntu-24.04"            CACHE PATH "Poco MySQL root" FORCE)
    SET(POCO_OPENSSL_ROOT_DIR "${CMAKE_PROJECT_ROOT_DIR}/../3rd-libs/openssl-3.0.20/x64-ubuntu-24.04"          CACHE PATH "Poco OpenSSL root" FORCE)
ENDIF()

SET(CMAKE_MODULE_PATH    "${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/cmake" ${CMAKE_MODULE_PATH})
SET(MYSQL_ROOT_DIR       "${POCO_MYSQL_ROOT_DIR}"           CACHE PATH "MySQL root" FORCE)
SET(MYSQL_INCLUDE_DIR    "${POCO_MYSQL_ROOT_DIR}/include"   CACHE PATH "MySQL include directory" FORCE)
SET(OPENSSL_ROOT_DIR     "${POCO_OPENSSL_ROOT_DIR}"         CACHE PATH "OpenSSL root" FORCE)
SET(OPENSSL_INCLUDE_DIR  "${POCO_OPENSSL_ROOT_DIR}/include" CACHE PATH "OpenSSL include directory" FORCE)

IF(WIN32)
    SET(MYSQL_LIBRARY          "${POCO_MYSQL_ROOT_DIR}/lib/libmysql.lib"              CACHE FILEPATH "MySQL client library" FORCE)
    SET(OPENSSL_SSL_LIBRARY    "${POCO_OPENSSL_ROOT_DIR}/lib/libssl.lib"              CACHE FILEPATH "OpenSSL SSL library" FORCE)
    SET(OPENSSL_CRYPTO_LIBRARY "${POCO_OPENSSL_ROOT_DIR}/lib/libcrypto.lib"           CACHE FILEPATH "OpenSSL Crypto library" FORCE)
ELSE()
    SET(MYSQL_LIBRARY          "${POCO_MYSQL_ROOT_DIR}/lib/libmysqlclient.so.21.2.45" CACHE FILEPATH "MySQL client library" FORCE)
    SET(OPENSSL_SSL_LIBRARY    "${POCO_OPENSSL_ROOT_DIR}/lib/libssl.so.3"             CACHE FILEPATH "OpenSSL SSL library" FORCE)
    SET(OPENSSL_CRYPTO_LIBRARY "${POCO_OPENSSL_ROOT_DIR}/lib/libcrypto.so.3"          CACHE FILEPATH "OpenSSL Crypto library" FORCE)
ENDIF()

SET(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL True)
FIND_PACKAGE(MySQL   REQUIRED)
FIND_PACKAGE(OpenSSL REQUIRED)
FIND_PACKAGE(Poco    REQUIRED COMPONENTS ${POCO_COMPONENTS})

INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../3rd-cmake/poco-definitions.cmake")

ADD_LIBRARY(poco_all INTERFACE)
SET(POCO_LINK_TARGETS)
FOREACH(POCO_COMPONENT ${POCO_COMPONENTS})
    LIST(APPEND POCO_LINK_TARGETS Poco::${POCO_COMPONENT})
ENDFOREACH()
TARGET_LINK_LIBRARIES(poco_all INTERFACE ${POCO_LINK_TARGETS})
TARGET_COMPILE_DEFINITIONS(poco_all INTERFACE ${POCO_EXTRA_COMPILE_DEFINITIONS})
SET_PROPERTY(GLOBAL PROPERTY PROJECT_POCO_LINK_TARGETS "${POCO_LINK_TARGETS}")

FUNCTION(PROJECT_GET_POCO_RUNTIME_COPY_COMMANDS OUTPUT_VARIABLE)
    GET_PROPERTY(POCO_RUNTIME_TARGETS GLOBAL PROPERTY PROJECT_POCO_LINK_TARGETS)
    IF(NOT POCO_RUNTIME_TARGETS)
        MESSAGE(FATAL_ERROR "Poco link targets are not configured.")
    ENDIF()

    STRING(TOUPPER "${CMAKE_BUILD_TYPE}" POCO_RUNTIME_CONFIG)
    SET(POCO_RUNTIME_COPY_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
    )

    FOREACH(POCO_RUNTIME_TARGET ${POCO_RUNTIME_TARGETS})
        GET_TARGET_PROPERTY(POCO_RUNTIME_LIB ${POCO_RUNTIME_TARGET} IMPORTED_LOCATION_${POCO_RUNTIME_CONFIG})
        IF(NOT POCO_RUNTIME_LIB)
            GET_TARGET_PROPERTY(POCO_RUNTIME_LIB
                ${POCO_RUNTIME_TARGET}
                IMPORTED_LOCATION)
        ENDIF()

        IF(NOT POCO_RUNTIME_LIB)
            MESSAGE(FATAL_ERROR "Poco runtime location is not defined: ${POCO_RUNTIME_TARGET}")
        ENDIF()

        GET_FILENAME_COMPONENT(POCO_RUNTIME_LIB_NAME "${POCO_RUNTIME_LIB}" NAME)
        LIST(APPEND POCO_RUNTIME_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${POCO_RUNTIME_LIB}"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
        )

        IF(NOT WIN32)
            IF(POCO_RUNTIME_LIB_NAME MATCHES "^(.*\\.so)\\..*$")
                SET(POCO_RUNTIME_LINK_NAME "${CMAKE_MATCH_1}")
                LIST(APPEND POCO_RUNTIME_COPY_COMMANDS
                    COMMAND ${CMAKE_COMMAND} -E remove -f
                        "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/${POCO_RUNTIME_LINK_NAME}"
                    COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                        ${CMAKE_COMMAND} -E create_symlink
                            "${POCO_RUNTIME_LIB_NAME}"
                            "${POCO_RUNTIME_LINK_NAME}"
                )
            ENDIF()
        ENDIF()
    ENDFOREACH()

    IF(WIN32)
        LIST(APPEND POCO_RUNTIME_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${POCO_MYSQL_ROOT_DIR}/lib/libmysql.dll"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${POCO_OPENSSL_ROOT_DIR}/lib/libssl-3-x64.dll"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${POCO_OPENSSL_ROOT_DIR}/lib/libcrypto-3-x64.dll"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
        )
    ELSE()
        LIST(APPEND POCO_RUNTIME_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${POCO_MYSQL_ROOT_DIR}/lib/libmysqlclient.so.21.2.45"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
            COMMAND ${CMAKE_COMMAND} -E remove -f
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/libmysqlclient.so.21"
            COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                ${CMAKE_COMMAND} -E create_symlink libmysqlclient.so.21.2.45 libmysqlclient.so.21
            COMMAND ${CMAKE_COMMAND} -E remove -f
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/libmysqlclient.so"
            COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                ${CMAKE_COMMAND} -E create_symlink libmysqlclient.so.21 libmysqlclient.so
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${POCO_OPENSSL_ROOT_DIR}/lib/libssl.so.3"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${POCO_OPENSSL_ROOT_DIR}/lib/libcrypto.so.3"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
            COMMAND ${CMAKE_COMMAND} -E remove -f
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/libssl.so"
            COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                ${CMAKE_COMMAND} -E create_symlink libssl.so.3 libssl.so
            COMMAND ${CMAKE_COMMAND} -E remove -f
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/libcrypto.so"
            COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                ${CMAKE_COMMAND} -E create_symlink libcrypto.so.3 libcrypto.so
        )
    ENDIF()

    SET(${OUTPUT_VARIABLE} ${POCO_RUNTIME_COPY_COMMANDS} PARENT_SCOPE)
ENDFUNCTION()

PROJECT_GET_POCO_RUNTIME_COPY_COMMANDS(POCO_RUNTIME_COPY_COMMANDS)
ADD_CUSTOM_TARGET(poco_runtime_copy
    ${POCO_RUNTIME_COPY_COMMANDS}
    VERBATIM
)
ADD_DEPENDENCIES(poco_all poco_runtime_copy)

IF(WIN32)
    SET_PROPERTY(TARGET poco_runtime_copy PROPERTY FOLDER "cmake")
ENDIF()

IF(WIN32 AND CMAKE_GENERATOR MATCHES "Visual Studio")
    SET(POCO_VSPROJ_BUILD_DIR "${CMAKE_PROJECT_ROOT_DIR}/3rd/poco/.build/windows/x64-${CMAKE_BUILD_TYPE}")

    FUNCTION(GET_POCO_EXTERNAL_MSPROJECT_PATH COMPONENT_NAME OUTPUT_VARIABLE)
        IF("${COMPONENT_NAME}" STREQUAL "NetSSL")
            SET(POCO_PROJECT_PATH "NetSSL_OpenSSL/NetSSL.vcxproj")
        ELSEIF("${COMPONENT_NAME}" STREQUAL "DataSQLite")
            SET(POCO_PROJECT_PATH "Data/SQLite/DataSQLite.vcxproj")
        ELSEIF("${COMPONENT_NAME}" STREQUAL "DataMySQL")
            SET(POCO_PROJECT_PATH "Data/MySQL/DataMySQL.vcxproj")
        ELSE()
            SET(POCO_PROJECT_PATH "${COMPONENT_NAME}/${COMPONENT_NAME}.vcxproj")
        ENDIF()

        SET(${OUTPUT_VARIABLE} "${POCO_PROJECT_PATH}" PARENT_SCOPE)
    ENDFUNCTION()

    FUNCTION(ADD_POCO_EXTERNAL_MSPROJECT TARGET_NAME PROJECT_PATH)
        SET(POCO_VSPROJ "${POCO_VSPROJ_BUILD_DIR}/${PROJECT_PATH}")

        IF(EXISTS "${POCO_VSPROJ}")
            INCLUDE_EXTERNAL_MSPROJECT(
                ${TARGET_NAME}
                "${POCO_VSPROJ}"
                PLATFORM x64
            )

            SET_PROPERTY(TARGET ${TARGET_NAME} PROPERTY FOLDER "3rd")
        ELSE()
            MESSAGE(WARNING
                "Poco vcxproj not found: ${POCO_VSPROJ}. "
                "Run 3rd/build.poco.bat ${CMAKE_BUILD_TYPE} before generating the main Visual Studio solution."
            )
        ENDIF()
    ENDFUNCTION()

    FOREACH(POCO_COMPONENT ${POCO_COMPONENTS})
        GET_POCO_EXTERNAL_MSPROJECT_PATH("${POCO_COMPONENT}" POCO_PROJECT_PATH)
        ADD_POCO_EXTERNAL_MSPROJECT("Poco${POCO_COMPONENT}" "${POCO_PROJECT_PATH}")
    ENDFOREACH()
ENDIF()
