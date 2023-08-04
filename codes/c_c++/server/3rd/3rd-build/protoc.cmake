# 项目名字
PROJECT(protoc)

# 头文件目录
SET(CURRENT_INCLUDE_DIR
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
)

# 宏定义、编译选项
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4244
        /wd4267
        /wd4996
    )
ELSE()
    # 宏定义
    SET(CURRENT_PRIVATE_COMPILE_DEFINITIONS
        # ...
    )

    SET(CURRENT_PUBLIC_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-sign-compare
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/code_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/command_line_interface.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_file.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_helpers.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_names.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_doc_comment.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_names.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_options.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/importer.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_kotlin_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_names.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/js/js_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_helpers.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/parser.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/php/php_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/plugin.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/plugin.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/python/python_generator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/ruby/ruby_generator.h
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/code_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/command_line_interface.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_enum.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_enum_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_extension.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_file.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_helpers.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_map_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_message.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_message_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_padding_optimizer.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_parse_function_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_primitive_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_service.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/cpp/cpp_string_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_doc_comment.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_enum.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_enum_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_field_base.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_helpers.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_map_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_message.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_message_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_primitive_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_reflection_class.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_repeated_enum_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_repeated_message_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_repeated_primitive_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_source_generator_base.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/csharp/csharp_wrapper_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_context.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_doc_comment.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_enum.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_enum_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_enum_field_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_enum_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_extension.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_extension_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_file.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_generator_factory.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_helpers.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_kotlin_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_map_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_map_field_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_message.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_message_builder.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_message_builder_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_message_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_message_field_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_message_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_name_resolver.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_primitive_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_primitive_field_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_service.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_shared_code_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_string_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/java/java_string_field_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/js/js_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/js/well_known_types_embed.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_enum.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_enum_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_extension.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_file.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_helpers.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_map_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_message.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_message_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_oneof.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/objectivec/objectivec_primitive_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/php/php_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/plugin.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/plugin.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/python/python_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/ruby/ruby_generator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/subprocess.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/zip_writer.cc
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      PUBLIC  protobuf)

# VS工程设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ENDIF()