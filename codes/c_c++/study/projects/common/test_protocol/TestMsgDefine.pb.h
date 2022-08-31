// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: TestMsgDefine.proto

#ifndef PROTOBUF_INCLUDED_TestMsgDefine_2eproto
#define PROTOBUF_INCLUDED_TestMsgDefine_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_TestMsgDefine_2eproto 

namespace protobuf_TestMsgDefine_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_TestMsgDefine_2eproto
namespace TestMsg {
}  // namespace TestMsg
namespace TestMsg {

enum CommonType {
  CT_FLOAT = 0,
  CT_DOUBLE = 1,
  CT_BYTES = 2,
  CT_INT32 = 3,
  CT_SINT32 = 4,
  CT_UINT32 = 5,
  CT_INT64 = 6,
  CT_SINT64 = 7,
  CT_UINT64 = 8,
  CommonType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  CommonType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool CommonType_IsValid(int value);
const CommonType CommonType_MIN = CT_FLOAT;
const CommonType CommonType_MAX = CT_UINT64;
const int CommonType_ARRAYSIZE = CommonType_MAX + 1;

const ::google::protobuf::EnumDescriptor* CommonType_descriptor();
inline const ::std::string& CommonType_Name(CommonType value) {
  return ::google::protobuf::internal::NameOfEnum(
    CommonType_descriptor(), value);
}
inline bool CommonType_Parse(
    const ::std::string& name, CommonType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<CommonType>(
    CommonType_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace TestMsg

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::TestMsg::CommonType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::TestMsg::CommonType>() {
  return ::TestMsg::CommonType_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_TestMsgDefine_2eproto
