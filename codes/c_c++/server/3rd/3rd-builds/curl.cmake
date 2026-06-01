IF(WIN32)
    SET(CURL_BUILD_DIR "${CMAKE_PROJECT_ROOT_DIR}/3rd/curl/.build/windows/x64-${CMAKE_BUILD_TYPE}/bin" CACHE PATH "curl build directory" FORCE)
ELSE()
    SET(CURL_BUILD_DIR "${CMAKE_PROJECT_ROOT_DIR}/3rd/curl/.build/linux/x64-${CMAKE_BUILD_TYPE}/bin"   CACHE PATH "curl build directory" FORCE)
ENDIF()

IF(WIN32)
    SET(CURL_IMPORTED_LOCATION "${CURL_BUILD_DIR}/libcurl.dll" CACHE FILEPATH "curl runtime library" FORCE)
    SET(CURL_IMPORTED_IMPLIB   "${CURL_BUILD_DIR}/libcurl.lib" CACHE FILEPATH "curl import library" FORCE)
ELSE()
    SET(CURL_IMPORTED_LOCATION "${CURL_BUILD_DIR}/libcurl.so.4.8.0" CACHE FILEPATH "curl runtime library" FORCE)
ENDIF()

SET(CURL_INCLUDE_DIR "${CMAKE_PROJECT_ROOT_DIR}/3rd/curl/include" CACHE PATH "curl include directory" FORCE)
IF(NOT EXISTS "${CURL_INCLUDE_DIR}/curl/curl.h")
    MESSAGE(FATAL_ERROR "curl header not found: ${CURL_INCLUDE_DIR}/curl/curl.h")
ENDIF()

IF(WIN32)
    IF(NOT EXISTS "${CURL_IMPORTED_LOCATION}")
        MESSAGE(FATAL_ERROR "curl runtime library not found: ${CURL_IMPORTED_LOCATION}")
    ENDIF()

    IF(NOT EXISTS "${CURL_IMPORTED_IMPLIB}")
        MESSAGE(FATAL_ERROR "curl import library not found: ${CURL_IMPORTED_IMPLIB}")
    ENDIF()
ELSE()
    IF(NOT EXISTS "${CURL_IMPORTED_LOCATION}")
        MESSAGE(FATAL_ERROR "curl runtime library not found: ${CURL_IMPORTED_LOCATION}")
    ENDIF()
ENDIF()

ADD_LIBRARY(curl SHARED IMPORTED GLOBAL)
SET_TARGET_PROPERTIES(curl PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIR}"
    IMPORTED_LOCATION             "${CURL_IMPORTED_LOCATION}"
)

IF(WIN32)
    SET_TARGET_PROPERTIES(curl PROPERTIES
        IMPORTED_IMPLIB "${CURL_IMPORTED_IMPLIB}"
    )
ELSE()
    SET_TARGET_PROPERTIES(curl PROPERTIES
        IMPORTED_SONAME "libcurl.so.4"
    )
ENDIF()

IF(NOT TARGET CURL::libcurl)
    ADD_LIBRARY(CURL::libcurl ALIAS curl)
ENDIF()

FUNCTION(PROJECT_GET_CURL_RUNTIME_COPY_COMMANDS OUTPUT_VARIABLE)
    SET(CURL_RUNTIME_COPY_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
    )

    IF(WIN32)
        LIST(APPEND CURL_RUNTIME_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CURL_IMPORTED_LOCATION}"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
        )
    ELSE()
        LIST(APPEND CURL_RUNTIME_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CURL_IMPORTED_LOCATION}"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
            COMMAND ${CMAKE_COMMAND} -E remove -f
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/libcurl.so.4"
            COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                ${CMAKE_COMMAND} -E create_symlink libcurl.so.4.8.0 libcurl.so.4
            COMMAND ${CMAKE_COMMAND} -E remove -f
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/libcurl.so"
            COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                ${CMAKE_COMMAND} -E create_symlink libcurl.so.4 libcurl.so
        )
    ENDIF()

    SET(${OUTPUT_VARIABLE} ${CURL_RUNTIME_COPY_COMMANDS} PARENT_SCOPE)
ENDFUNCTION()

PROJECT_GET_CURL_RUNTIME_COPY_COMMANDS(CURL_RUNTIME_COPY_COMMANDS)
ADD_CUSTOM_TARGET(curl_runtime_copy
    ${CURL_RUNTIME_COPY_COMMANDS}
    VERBATIM
)
ADD_DEPENDENCIES(curl curl_runtime_copy)

IF(WIN32)
    SET_PROPERTY(TARGET curl_runtime_copy PROPERTY FOLDER "cmake")
ENDIF()

IF(WIN32 AND CMAKE_GENERATOR MATCHES "Visual Studio")
    SET(CURL_VSPROJ_BUILD_DIR "${CMAKE_PROJECT_ROOT_DIR}/3rd/curl/.build/windows/x64-${CMAKE_BUILD_TYPE}")

    FUNCTION(ADD_CURL_EXTERNAL_MSPROJECT TARGET_NAME PROJECT_PATH)
        SET(CURL_VSPROJ "${CURL_VSPROJ_BUILD_DIR}/${PROJECT_PATH}")

        IF(EXISTS "${CURL_VSPROJ}")
            INCLUDE_EXTERNAL_MSPROJECT(
                ${TARGET_NAME}
                "${CURL_VSPROJ}"
                PLATFORM x64
            )

            SET_PROPERTY(TARGET ${TARGET_NAME} PROPERTY FOLDER "3rd/curl")
        ELSE()
            MESSAGE(WARNING
                "curl vcxproj not found: ${CURL_VSPROJ}. "
                "Run 3rd/3rd-cmake/curl.build.bat ${CMAKE_BUILD_TYPE} before generating the main Visual Studio solution."
            )
        ENDIF()
    ENDFUNCTION()

    ADD_CURL_EXTERNAL_MSPROJECT(libcurl_object "lib/libcurl_object.vcxproj")
    ADD_CURL_EXTERNAL_MSPROJECT(libcurl_shared "lib/libcurl_shared.vcxproj")
ENDIF()
