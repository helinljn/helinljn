# 项目名字
SET(CURRENT_TARGET_NAME opencc)

# 头文件目录
SET(CURRENT_PRIVATE_INCLUDE_DIRS
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/marisa-0.2.6/include
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/marisa-0.2.6/lib
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/rapidjson-1.1.0
)

SET(CURRENT_PUBLIC_INCLUDE_DIRS
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src
)

# 宏定义、编译选项、链接库
IF(WIN32)
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -Dlibopencc_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4267
        /wd4702
        /wd4996
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        # ...
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        -Dlibopencc_EXPORTS
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-class-memaccess
        -Wno-deprecated-declarations
    )

    # 链接库
    SET(CURRENT_LINK_LIBS
        # ...
    )
ENDIF()

# opencc源文件
SET(OPENCC_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Common.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Config.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Conversion.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/ConversionChain.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Converter.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Dict.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/DictConverter.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/DictEntry.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/DictGroup.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Exception.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Export.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Lexicon.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/MarisaDict.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/MaxMatchSegmentation.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Optional.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/PhraseExtract.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Segmentation.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Segments.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/SerializableDict.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/SerializedValues.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/SimpleConverter.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/TextDict.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/UTF8StringSlice.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/UTF8Util.hpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/opencc.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/opencc_config.h

    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Config.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Conversion.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/ConversionChain.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Converter.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Dict.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/DictConverter.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/DictEntry.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/DictGroup.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Lexicon.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/MarisaDict.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/MaxMatchSegmentation.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/PhraseExtract.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/SerializedValues.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/SimpleConverter.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/Segmentation.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/TextDict.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/UTF8StringSlice.cpp
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src/UTF8Util.cpp
)

# marisa源文件
FILE(GLOB_RECURSE MARISA_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/marisa-0.2.6/include/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/marisa-0.2.6/lib/*.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/marisa-0.2.6/lib/*.cc
)

# rapidjson源文件
FILE(GLOB_RECURSE RAPIDJSON_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/rapidjson-1.1.0/*.h
)

# 生成动态库
ADD_LIBRARY(${CURRENT_TARGET_NAME} SHARED ${OPENCC_SRC_LIST} ${MARISA_SRC_LIST} ${RAPIDJSON_SRC_LIST})
PROJECT_TARGET_APPLY_COMMON_OPTIONS(${CURRENT_TARGET_NAME})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_TARGET_NAME} PRIVATE ${CURRENT_PRIVATE_INCLUDE_DIRS})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_TARGET_NAME} PUBLIC  ${CURRENT_PUBLIC_INCLUDE_DIRS})
TARGET_COMPILE_DEFINITIONS(${CURRENT_TARGET_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${CURRENT_TARGET_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${CURRENT_TARGET_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_DIRECTORIES(${CURRENT_TARGET_NAME}    PRIVATE ${PROJECT_DEBUGGER_WORKING_DIRECTORY})
TARGET_LINK_LIBRARIES(${CURRENT_TARGET_NAME}      PUBLIC  ${CURRENT_LINK_LIBS})

# 其它设置
IF(WIN32)
    # MSVC运行库设置
    SET_PROPERTY(TARGET ${CURRENT_TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # VS工程设置
    SET_PROPERTY(TARGET ${CURRENT_TARGET_NAME} PROPERTY FOLDER "projects")
    SET_PROPERTY(TARGET ${CURRENT_TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DEBUGGER_WORKING_DIRECTORY}")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/src PREFIX "opencc"
        FILES ${OPENCC_SRC_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/marisa-0.2.6 PREFIX "marisa"
        FILES ${MARISA_SRC_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/opencc/deps/rapidjson-1.1.0/rapidjson PREFIX "rapidjson"
        FILES ${RAPIDJSON_SRC_LIST})
ENDIF()