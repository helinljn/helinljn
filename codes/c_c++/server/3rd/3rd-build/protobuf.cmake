# 项目名字
PROJECT(protobuf)

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
        /wd4018
        /wd4065
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
        -Wno-stringop-overflow
    )
ENDIF()

# 头文件
FILE(GLOB_RECURSE CURRENT_DIR_INCLUDE_LIST
    # protobuf-lite
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/any.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/arena.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/arena_impl.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/arenastring.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/explicitly_constructed.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/extension_set.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/extension_set_inl.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_enum_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_table_driven.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_table_driven_lite.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_tctable_decl.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_tctable_impl.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_tctable_impl.inc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/has_bits.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/implicit_weak_message.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/inlined_string_field.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/coded_stream.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/io_win32.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/strtod.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/zero_copy_stream.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/zero_copy_stream_impl.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/zero_copy_stream_impl_lite.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map_entry_lite.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map_field_lite.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map_type_handler.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/message_lite.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/metadata_lite.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/parse_context.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/port.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/repeated_field.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/repeated_ptr_field.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/bytestream.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/callback.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/casts.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/common.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/hash.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/logging.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/macros.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/map_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/mutex.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/once.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/platform_macros.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/port.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/status.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/stl_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/stringpiece.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/strutil.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/stubs/template_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/wire_format_lite.h

    # protobuf
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/any.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/api.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/importer.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/compiler/parser.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/descriptor.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/descriptor.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/descriptor_database.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/duration.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/dynamic_message.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/empty.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/field_access_listener.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/field_mask.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_enum_reflection.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_bases.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_reflection.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/gzip_stream.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/printer.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/io/tokenizer.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map_entry.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map_field.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/map_field_inl.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/message.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/metadata.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/reflection.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/reflection_ops.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/service.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/source_context.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/struct.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/text_format.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/timestamp.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/type.pb.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/unknown_field_set.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/delimited_message_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/field_comparator.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/field_mask_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/json_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/message_differencer.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/time_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/type_resolver.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/type_resolver_util.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/wire_format.h
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/wrappers.pb.h
)

# 源文件
FILE(GLOB_RECURSE CURRENT_DIR_SRC_LIST
    # protobuf-lite
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/any_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/arena.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/arenastring.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/extension_set.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_enum_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_table_driven_lite.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_tctable_lite.cc
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
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/repeated_ptr_field.cc
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

    # protobuf
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
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/generated_message_tctable_full.cc
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
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/internal/utility.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/json_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/message_differencer.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/time_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/util/type_resolver_util.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/wire_format.cc
    ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf/wrappers.pb.cc
)

# 生成静态库
ADD_LIBRARY(${PROJECT_NAME}                STATIC  ${CURRENT_DIR_INCLUDE_LIST} ${CURRENT_DIR_SRC_LIST})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CURRENT_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ${CURRENT_PRIVATE_COMPILE_DEFINITIONS})
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC  ${CURRENT_PUBLIC_COMPILE_DEFINITIONS})
TARGET_COMPILE_OPTIONS(${PROJECT_NAME}     PRIVATE ${CURRENT_COMPILE_OPTIONS})

# VS工程设置
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "3rd")

    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf PREFIX "include"
        FILES ${CURRENT_DIR_INCLUDE_LIST})
    SOURCE_GROUP(TREE ${CMAKE_PROJECT_ROOT_DIR}/3rd/protobuf/src/google/protobuf PREFIX "src"
        FILES ${CURRENT_DIR_SRC_LIST})
ENDIF()