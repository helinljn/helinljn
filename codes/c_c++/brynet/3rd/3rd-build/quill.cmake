# 项目名字
PROJECT(quill)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/fmt/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include
)

# 宏定义、编译选项、链接库
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DQUILL_DLL_EXPORT
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        -DQUILL_BUILD_SHARED
        -DQUILL_FMT_EXTERNAL
        -DNOMINMAX
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4100
        /wd4324
        /wd4702
        /wd4996
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        fmt
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -DQUILL_DLL_EXPORT
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        -DQUILL_BUILD_SHARED
        -DQUILL_FMT_EXTERNAL
        -DNOMINMAX
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-pedantic
        -Wno-unused-parameter
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        fmt
    )
ENDIF()

# 头文件
SET(CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/backend/BackendWorker.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/backend/BacktraceStorage.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/backend/StringFromTime.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/backend/TimestampFormatter.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/backend/TransitEventBuffer.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/misc/Attributes.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/misc/Common.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/misc/FileUtilities.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/misc/Os.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/misc/Rdtsc.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/misc/RdtscClock.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/misc/TypeTraitsCopyable.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/misc/Utilities.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/spsc_queue/BoundedQueue.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/spsc_queue/UnboundedQueue.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/HandlerCollection.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/LoggerCollection.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/LoggerDetails.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/LogMacros.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/LogManager.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/Serialize.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/ThreadContext.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/ThreadContextCollection.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/detail/SignalHandler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/clock/TimestampClock.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/filters/FilterBase.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/handlers/ConsoleHandler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/handlers/FileHandler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/handlers/Handler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/handlers/JsonFileHandler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/handlers/NullHandler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/handlers/RotatingFileHandler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/handlers/StreamHandler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/Config.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/Fmt.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/Logger.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/LogLevel.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/MacroMetadata.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/PatternFormatter.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/Quill.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/QuillError.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/TransitEvent.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/Clock.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/TweakMe.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include/quill/Utility.h
)

# 源文件
SET(CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/backend/BackendWorker.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/backend/BacktraceStorage.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/backend/TimestampFormatter.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/backend/StringFromTime.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/backend/TransitEventBuffer.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/misc/FileUtilities.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/misc/Os.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/misc/RdtscClock.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/misc/Utilities.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/HandlerCollection.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/LoggerCollection.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/SignalHandler.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/ThreadContext.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/detail/ThreadContextCollection.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/handlers/ConsoleHandler.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/handlers/FileHandler.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/handlers/Handler.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/handlers/JsonFileHandler.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/handlers/RotatingFileHandler.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/handlers/StreamHandler.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/LogLevel.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/PatternFormatter.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/Quill.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src/Utility.cpp
)

# 生成动态库
ADD_LIBRARY(${PROJECT_NAME}                SHARED  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${PROJECT_NAME}    PRIVATE ${CMAKE_PROJECT_BUILD_ROOT_DIR}/${CMAKE_BUILD_TYPE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PUBLIC  ${CURRENT_LINK_LIBS})

# 其它设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # MSVC运行库设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # 导出所有符号设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS True)
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS True)

    # VS工程设置
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/include PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/quill/quill/src PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ENDIF()