// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: TestMsgDefine.proto

#include "TestMsgDefine.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace PBMsg {
}  // namespace PBMsg
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_TestMsgDefine_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_TestMsgDefine_2eproto = nullptr;
const uint32_t TableStruct_TestMsgDefine_2eproto::offsets[1] = {};
static constexpr ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema* schemas = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::Message* const* file_default_instances = nullptr;

const char descriptor_table_protodef_TestMsgDefine_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\023TestMsgDefine.proto\022\005PBMsg*\240\001\n\016TestCom"
  "monType\022\013\n\007CT_BOOL\020\000\022\014\n\010CT_FLOAT\020\001\022\r\n\tCT"
  "_DOUBLE\020\002\022\014\n\010CT_BYTES\020\003\022\014\n\010CT_INT32\020\004\022\r\n"
  "\tCT_SINT32\020\005\022\r\n\tCT_UINT32\020\006\022\014\n\010CT_INT64\020"
  "\007\022\r\n\tCT_SINT64\020\010\022\r\n\tCT_UINT64\020\tb\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_TestMsgDefine_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_TestMsgDefine_2eproto = {
  false, false, 199, descriptor_table_protodef_TestMsgDefine_2eproto, "TestMsgDefine.proto", 
  &descriptor_table_TestMsgDefine_2eproto_once, nullptr, 0, 0,
  schemas, file_default_instances, TableStruct_TestMsgDefine_2eproto::offsets,
  nullptr, file_level_enum_descriptors_TestMsgDefine_2eproto, file_level_service_descriptors_TestMsgDefine_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_TestMsgDefine_2eproto_getter() {
  return &descriptor_table_TestMsgDefine_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_TestMsgDefine_2eproto(&descriptor_table_TestMsgDefine_2eproto);
namespace PBMsg {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* TestCommonType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_TestMsgDefine_2eproto);
  return file_level_enum_descriptors_TestMsgDefine_2eproto[0];
}
bool TestCommonType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace PBMsg
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
