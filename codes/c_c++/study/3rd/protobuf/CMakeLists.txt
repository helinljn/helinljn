# 项目名字
PROJECT(protobuf)

# 设置编译目标
SET(CURRENT_PROTOBUF_LITE_NAME "protobuf-lite")
SET(CURRENT_PROTOBUF_NAME      "protobuf")
SET(CURRENT_PROTOC_NAME        "protoc")

# 将编译目标添加至依赖列表，用于生成可执行文件时的编译依赖
SET(CURRENT_DEPENDENT_LIBS_LIST "${CURRENT_DEPENDENT_LIBS_LIST};${CURRENT_PROTOBUF_LITE_NAME}" CACHE STRING INTERNAL FORCE)
SET(CURRENT_DEPENDENT_LIBS_LIST "${CURRENT_DEPENDENT_LIBS_LIST};${CURRENT_PROTOBUF_NAME}"      CACHE STRING INTERNAL FORCE)
SET(CURRENT_DEPENDENT_LIBS_LIST "${CURRENT_DEPENDENT_LIBS_LIST};${CURRENT_PROTOC_NAME}"        CACHE STRING INTERNAL FORCE)

# 源文件 - protobuf-lite
SET(CURRENT_PROTO_LITE_SRC_LIST
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/any_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/arena.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/arenastring.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/extension_set.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/generated_enum_util.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/generated_message_table_driven_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/generated_message_util.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/implicit_weak_message.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/coded_stream.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/io_win32.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/strtod.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream_impl.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/map.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/message_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/parse_context.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/repeated_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/bytestream.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/common.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/int128.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/status.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/statusor.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/stringpiece.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/stringprintf.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/structurally_valid.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/strutil.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/time.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/wire_format_lite.cc
)

# 源文件 - protobuf
SET(CURRENT_PROTOBUF_SRC_LIST
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/any.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/any.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/api.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/importer.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/parser.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/descriptor.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/descriptor.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/descriptor_database.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/duration.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/dynamic_message.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/empty.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/extension_set_heavy.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/field_mask.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/generated_message_reflection.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/generated_message_table_driven.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/gzip_stream.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/printer.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/io/tokenizer.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/map_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/message.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/reflection_ops.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/service.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/source_context.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/struct.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/stubs/substitute.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/text_format.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/timestamp.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/type.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/unknown_field_set.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/delimited_message_util.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/field_comparator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/field_mask_util.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/datapiece.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/default_value_objectwriter.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/error_listener.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/field_mask_utility.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/json_escaping.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/json_objectwriter.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/json_stream_parser.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/object_writer.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/proto_writer.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/protostream_objectsource.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/protostream_objectwriter.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/type_info.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/type_info_test_helper.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/internal/utility.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/json_util.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/message_differencer.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/time_util.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/util/type_resolver_util.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/wire_format.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/wrappers.pb.cc
)

# 源文件 - protoc
SET(CURRENT_PROTOC_SRC_LIST
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/code_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/command_line_interface.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_enum.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_enum_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_extension.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_file.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_helpers.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_map_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_message.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_message_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_padding_optimizer.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_primitive_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_service.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/cpp/cpp_string_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_doc_comment.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_enum.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_enum_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_field_base.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_helpers.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_map_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_message.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_message_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_primitive_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_reflection_class.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_repeated_enum_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_repeated_message_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_repeated_primitive_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_source_generator_base.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/csharp/csharp_wrapper_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_context.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_doc_comment.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_enum.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_enum_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_enum_field_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_enum_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_extension.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_extension_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_file.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_generator_factory.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_helpers.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_map_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_map_field_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_message.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_message_builder.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_message_builder_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_message_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_message_field_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_message_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_name_resolver.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_primitive_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_primitive_field_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_service.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_shared_code_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_string_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/java/java_string_field_lite.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/js/js_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/js/well_known_types_embed.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_enum.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_enum_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_extension.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_file.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_helpers.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_map_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_message.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_message_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_oneof.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/objectivec/objectivec_primitive_field.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/php/php_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/plugin.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/plugin.pb.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/python/python_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/ruby/ruby_generator.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/subprocess.cc
    ${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/zip_writer.cc
)

# 头文件目录、宏定义、编译选项
IF(MSVC)
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
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
        /wd4702
        /wd4800
        /wd4996
    )
ELSE()
    # 头文件目录
    SET(CURRENT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
    )

    # 宏定义
    SET(CURRENT_COMPILE_DEFINITIONS
        -DHAVE_PTHREAD
    )

    # 编译选项
    SET(CURRENT_COMPILE_OPTIONS
        -Wno-unused-parameter
        -Wno-unused-function
        -Wno-redundant-move
        -Wno-sign-compare
        -Wno-type-limits
        -Wno-pedantic
        -Wno-missing-field-initializers
        -Wno-stringop-overflow
    )
ENDIF()

# 生成静态库 - protobuf-lite
ADD_LIBRARY(${CURRENT_PROTOBUF_LITE_NAME}                STATIC  ${CURRENT_PROTO_LITE_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_PROTOBUF_LITE_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${CURRENT_PROTOBUF_LITE_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${CURRENT_PROTOBUF_LITE_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

# 调试信息 - protobuf-lite
MESSAGE(STATUS "---------------------------------------------------------------------")
MESSAGE(STATUS "CMAKE_PROJECT_NAME          = ${CMAKE_PROJECT_NAME}")
MESSAGE(STATUS "PROJECT_NAME                = ${CURRENT_PROTOBUF_LITE_NAME}")
MESSAGE(STATUS "PROJECT_SOURCE_DIR          = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR          = ${PROJECT_BINARY_DIR}")
MESSAGE(STATUS "CURRENT_PROTO_LITE_SRC_LIST = ${CURRENT_PROTO_LITE_SRC_LIST}")
MESSAGE(STATUS "CURRENT_INCLUDE_DIR         = ${CURRENT_INCLUDE_DIR}")
MESSAGE(STATUS "CURRENT_COMPILE_DEFINITIONS = ${CURRENT_COMPILE_DEFINITIONS}")
MESSAGE(STATUS "CURRENT_COMPILE_OPTIONS     = ${CURRENT_COMPILE_OPTIONS}")

# 生成静态库 - protobuf
ADD_LIBRARY(${CURRENT_PROTOBUF_NAME}                STATIC  ${CURRENT_PROTO_LITE_SRC_LIST} ${CURRENT_PROTOBUF_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_PROTOBUF_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${CURRENT_PROTOBUF_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${CURRENT_PROTOBUF_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

# 调试信息 - protobuf
MESSAGE(STATUS "---------------------------------------------------------------------")
MESSAGE(STATUS "CMAKE_PROJECT_NAME          = ${CMAKE_PROJECT_NAME}")
MESSAGE(STATUS "PROJECT_NAME                = ${CURRENT_PROTOBUF_NAME}")
MESSAGE(STATUS "PROJECT_SOURCE_DIR          = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR          = ${PROJECT_BINARY_DIR}")
MESSAGE(STATUS "CURRENT_PROTOBUF_SRC_LIST   = ${CURRENT_PROTO_LITE_SRC_LIST} ${CURRENT_PROTOBUF_SRC_LIST}")
MESSAGE(STATUS "CURRENT_INCLUDE_DIR         = ${CURRENT_INCLUDE_DIR}")
MESSAGE(STATUS "CURRENT_COMPILE_DEFINITIONS = ${CURRENT_COMPILE_DEFINITIONS}")
MESSAGE(STATUS "CURRENT_COMPILE_OPTIONS     = ${CURRENT_COMPILE_OPTIONS}")

# 生成静态库 - protoc
ADD_LIBRARY(${CURRENT_PROTOC_NAME}                STATIC  ${CURRENT_PROTOC_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${CURRENT_PROTOC_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${CURRENT_PROTOC_NAME} PRIVATE ${CURRENT_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${CURRENT_PROTOC_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

# 调试信息 - protoc
MESSAGE(STATUS "---------------------------------------------------------------------")
MESSAGE(STATUS "CMAKE_PROJECT_NAME          = ${CMAKE_PROJECT_NAME}")
MESSAGE(STATUS "PROJECT_NAME                = ${CURRENT_PROTOC_NAME}")
MESSAGE(STATUS "PROJECT_SOURCE_DIR          = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR          = ${PROJECT_BINARY_DIR}")
MESSAGE(STATUS "CURRENT_PROTOC_SRC_LIST     = ${CURRENT_PROTOC_SRC_LIST}")
MESSAGE(STATUS "CURRENT_INCLUDE_DIR         = ${CURRENT_INCLUDE_DIR}")
MESSAGE(STATUS "CURRENT_COMPILE_DEFINITIONS = ${CURRENT_COMPILE_DEFINITIONS}")
MESSAGE(STATUS "CURRENT_COMPILE_OPTIONS     = ${CURRENT_COMPILE_OPTIONS}")