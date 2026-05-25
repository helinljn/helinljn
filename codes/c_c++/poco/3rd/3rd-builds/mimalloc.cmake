STRING(TOLOWER "${CMAKE_BUILD_TYPE}" MIMALLOC_BUILD_TYPE_LC)
IF(MIMALLOC_BUILD_TYPE_LC MATCHES "^(release|relwithdebinfo|minsizerel|none)$")
    SET(MIMALLOC_LIB_NAME "mimalloc")
ELSE()
    SET(MIMALLOC_LIB_NAME "mimalloc-${MIMALLOC_BUILD_TYPE_LC}")
ENDIF()

IF(WIN32)
    SET(MIMALLOC_BUILD_DIR    "${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/.build/windows/x64-${CMAKE_BUILD_TYPE}" CACHE PATH "mimalloc build directory" FORCE)
    SET(MIMALLOC_RUNTIME_LIB  "${MIMALLOC_BUILD_DIR}/bin/${MIMALLOC_LIB_NAME}.dll"                            CACHE FILEPATH "mimalloc runtime library" FORCE)
    SET(MIMALLOC_IMPORT_LIB   "${MIMALLOC_BUILD_DIR}/bin/${MIMALLOC_LIB_NAME}.dll.lib"                        CACHE FILEPATH "mimalloc import library" FORCE)
    SET(MIMALLOC_REDIRECT_DLL "${MIMALLOC_BUILD_DIR}/bin/mimalloc-redirect.dll"                               CACHE FILEPATH "mimalloc redirect runtime" FORCE)
ELSE()
    SET(MIMALLOC_BUILD_DIR   "${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/.build/linux/x64-${CMAKE_BUILD_TYPE}" CACHE PATH "mimalloc build directory" FORCE)
    SET(MIMALLOC_RUNTIME_LIB "${MIMALLOC_BUILD_DIR}/bin/lib${MIMALLOC_LIB_NAME}.so.2.3"                    CACHE FILEPATH "mimalloc runtime library" FORCE)
    SET(MIMALLOC_SONAME      "lib${MIMALLOC_LIB_NAME}.so.2"                                                CACHE STRING "mimalloc runtime soname" FORCE)
    SET(MIMALLOC_LINK_NAME   "lib${MIMALLOC_LIB_NAME}.so"                                                  CACHE STRING "mimalloc runtime link name" FORCE)
ENDIF()

SET(MIMALLOC_INCLUDE_DIR "${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/include" CACHE PATH "mimalloc include directory" FORCE)

ADD_LIBRARY(mimalloc_shared SHARED IMPORTED GLOBAL)
SET_TARGET_PROPERTIES(mimalloc_shared PROPERTIES
    IMPORTED_LOCATION "${MIMALLOC_RUNTIME_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${MIMALLOC_INCLUDE_DIR}"
)

IF(WIN32)
    SET_TARGET_PROPERTIES(mimalloc_shared PROPERTIES
        IMPORTED_IMPLIB "${MIMALLOC_IMPORT_LIB}"
    )
ELSE()
    SET_TARGET_PROPERTIES(mimalloc_shared PROPERTIES
        IMPORTED_SONAME "${MIMALLOC_SONAME}"
    )
ENDIF()

ADD_LIBRARY(mimalloc INTERFACE)
TARGET_LINK_LIBRARIES(mimalloc INTERFACE mimalloc_shared)

FUNCTION(PROJECT_GET_MIMALLOC_RUNTIME_COPY_COMMANDS OUTPUT_VARIABLE)
    IF(NOT TARGET mimalloc_shared)
        MESSAGE(FATAL_ERROR "Target is not defined: mimalloc_shared")
    ENDIF()

    STRING(TOUPPER "${CMAKE_BUILD_TYPE}" MIMALLOC_RUNTIME_CONFIG)
    SET(MIMALLOC_RUNTIME_COPY_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
    )

    GET_TARGET_PROPERTY(MIMALLOC_RUNTIME_LIB mimalloc_shared IMPORTED_LOCATION_${MIMALLOC_RUNTIME_CONFIG})
    IF(NOT MIMALLOC_RUNTIME_LIB)
        GET_TARGET_PROPERTY(MIMALLOC_RUNTIME_LIB
            mimalloc_shared
            IMPORTED_LOCATION)
    ENDIF()

    IF(NOT MIMALLOC_RUNTIME_LIB)
        MESSAGE(FATAL_ERROR "Mimalloc runtime location is not defined.")
    ENDIF()

    GET_FILENAME_COMPONENT(MIMALLOC_RUNTIME_LIB_NAME "${MIMALLOC_RUNTIME_LIB}" NAME)
    LIST(APPEND MIMALLOC_RUNTIME_COPY_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MIMALLOC_RUNTIME_LIB}"
            "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
    )

    IF(WIN32)
        LIST(APPEND MIMALLOC_RUNTIME_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MIMALLOC_REDIRECT_DLL}"
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
        )
    ELSE()
        GET_TARGET_PROPERTY(MIMALLOC_RUNTIME_SONAME mimalloc_shared IMPORTED_SONAME)
        IF(NOT MIMALLOC_RUNTIME_SONAME)
            SET(MIMALLOC_RUNTIME_SONAME "lib${MIMALLOC_LIB_NAME}.so.2")
        ENDIF()

        LIST(APPEND MIMALLOC_RUNTIME_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E remove -f
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/${MIMALLOC_RUNTIME_SONAME}"
            COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                ${CMAKE_COMMAND} -E create_symlink
                    "${MIMALLOC_RUNTIME_LIB_NAME}"
                    "${MIMALLOC_RUNTIME_SONAME}"
            COMMAND ${CMAKE_COMMAND} -E remove -f
                "${PROJECT_DEBUGGER_WORKING_DIRECTORY}/${MIMALLOC_LINK_NAME}"
            COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_DEBUGGER_WORKING_DIRECTORY}"
                ${CMAKE_COMMAND} -E create_symlink
                    "${MIMALLOC_RUNTIME_SONAME}"
                    "${MIMALLOC_LINK_NAME}"
        )
    ENDIF()

    SET(${OUTPUT_VARIABLE} ${MIMALLOC_RUNTIME_COPY_COMMANDS} PARENT_SCOPE)
ENDFUNCTION()

PROJECT_GET_MIMALLOC_RUNTIME_COPY_COMMANDS(MIMALLOC_RUNTIME_COPY_COMMANDS)
ADD_CUSTOM_TARGET(mimalloc_runtime_copy
    ${MIMALLOC_RUNTIME_COPY_COMMANDS}
    VERBATIM
)
ADD_DEPENDENCIES(mimalloc mimalloc_runtime_copy)

IF(WIN32)
    SET_PROPERTY(TARGET mimalloc_runtime_copy PROPERTY FOLDER "cmake")
ENDIF()

IF(WIN32 AND CMAKE_GENERATOR MATCHES "Visual Studio")
    SET(MIMALLOC_VSPROJ_BUILD_DIR "${CMAKE_PROJECT_ROOT_DIR}/3rd/mimalloc/.build/windows/x64-${CMAKE_BUILD_TYPE}")

    FUNCTION(ADD_MIMALLOC_EXTERNAL_MSPROJECT TARGET_NAME PROJECT_PATH)
        SET(MIMALLOC_VSPROJ "${MIMALLOC_VSPROJ_BUILD_DIR}/${PROJECT_PATH}")

        IF(EXISTS "${MIMALLOC_VSPROJ}")
            INCLUDE_EXTERNAL_MSPROJECT(
                ${TARGET_NAME}
                "${MIMALLOC_VSPROJ}"
                PLATFORM x64
            )

            SET_PROPERTY(TARGET ${TARGET_NAME} PROPERTY FOLDER "mimalloc")
            ADD_DEPENDENCIES(mimalloc_runtime_copy ${TARGET_NAME})
        ELSE()
            MESSAGE(WARNING
                "Mimalloc vcxproj not found: ${MIMALLOC_VSPROJ}. "
                "Run 3rd/mimalloc.build.bat ${CMAKE_BUILD_TYPE} before generating the main Visual Studio solution."
            )
        ENDIF()
    ENDFUNCTION()

    ADD_MIMALLOC_EXTERNAL_MSPROJECT(MimallocExternal "mimalloc.vcxproj")
ENDIF()
