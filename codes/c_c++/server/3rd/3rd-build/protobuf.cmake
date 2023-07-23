# 项目名字
PROJECT(protobuf)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        # ...
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        /wd4018
        /wd4065
        /wd4100
        /wd4125
        /wd4127
        /wd4146
        /wd4244
        /wd4251
        /wd4267
        /wd4305
        /wd4307
        /wd4309
        /wd4310
        /wd4334
        /wd4355
        /wd4389
        /wd4456
        /wd4457
        /wd4506
        /wd4701
        /wd4702
        /wd4703
        /wd4800
        /wd4996
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DHAVE_PTHREAD
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-pedantic
        -Wno-sign-compare
        -Wno-redundant-move
        -Wno-unused-parameter
        -Wno-stringop-overflow
    )
ENDIF()

# 源文件 - protobuf-lite
SET(CURRENT_PROTO_LITE_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/any_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/arena.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/arenastring.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/extension_set.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_enum_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_table_driven_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/implicit_weak_message.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/inlined_string_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/coded_stream.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/io_win32.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/strtod.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/zero_copy_stream.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/zero_copy_stream_impl.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/message_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/parse_context.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/repeated_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/bytestream.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/common.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/int128.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/status.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/statusor.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/stringpiece.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/stringprintf.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/structurally_valid.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/strutil.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/time.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/wire_format_lite.cc
)

# 源文件 - protobuf
SET(CURRENT_PROTOBUF_SRC_LIST
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/any.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/any.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/api.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/importer.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/parser.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/descriptor.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/descriptor.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/descriptor_database.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/duration.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/dynamic_message.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/empty.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/extension_set_heavy.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/field_mask.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_bases.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_reflection.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_table_driven.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/gzip_stream.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/printer.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/tokenizer.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map_field.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/message.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/reflection_ops.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/service.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/source_context.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/struct.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/substitute.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/text_format.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/timestamp.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/type.pb.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/unknown_field_set.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/delimited_message_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/field_comparator.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/field_mask_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/datapiece.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/default_value_objectwriter.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/error_listener.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/field_mask_utility.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/json_escaping.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/json_objectwriter.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/json_stream_parser.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/object_writer.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/proto_writer.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/protostream_objectsource.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/protostream_objectwriter.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/type_info.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/type_info_test_helper.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/utility.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/json_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/message_differencer.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/time_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/type_resolver_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/wire_format.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/wrappers.pb.cc
)

# 源文件 - protoc
SET(CURRENT_PROTOC_SRC_LIST
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

# 目标名
SET(CURRENT_PROTOBUF_LITE_NAME "protobuf-lite")
SET(CURRENT_PROTOBUF_NAME      "protobuf")
SET(CURRENT_PROTOC_NAME        "protoc")

# 生成静态库 - protobuf-lite
ADD_LIBRARY(${CURRENT_PROTOBUF_LITE_NAME}                STATIC  ${CURRENT_PROTO_LITE_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_PROTOBUF_LITE_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${CURRENT_PROTOBUF_LITE_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${CURRENT_PROTOBUF_LITE_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

# 生成静态库 - protobuf
ADD_LIBRARY(${CURRENT_PROTOBUF_NAME}                STATIC  ${CURRENT_PROTO_LITE_SRC_LIST} ${CURRENT_PROTOBUF_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_PROTOBUF_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${CURRENT_PROTOBUF_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${CURRENT_PROTOBUF_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

# 生成静态库 - protoc
ADD_LIBRARY(${CURRENT_PROTOC_NAME}                STATIC  ${CURRENT_PROTOC_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_PROTOC_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${CURRENT_PROTOC_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${CURRENT_PROTOC_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})