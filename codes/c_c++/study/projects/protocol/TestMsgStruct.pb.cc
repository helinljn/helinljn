// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: TestMsgStruct.proto

#include "TestMsgStruct.pb.h"

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
namespace TestMsg {
constexpr TestCommonInfo::TestCommonInfo(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : test_string_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , test_double_(0)
  , test_float_(0)
  , test_int32_(0)
  , test_sint32_(0)
  , test_uint32_(0u)
  , test_int64_(PROTOBUF_LONGLONG(0))
  , test_sint64_(PROTOBUF_LONGLONG(0))
  , test_uint64_(PROTOBUF_ULONGLONG(0)){}
struct TestCommonInfoDefaultTypeInternal {
  constexpr TestCommonInfoDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~TestCommonInfoDefaultTypeInternal() {}
  union {
    TestCommonInfo _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT TestCommonInfoDefaultTypeInternal _TestCommonInfo_default_instance_;
constexpr TestCommonInfoList::TestCommonInfoList(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : common_info_list_()
  , common_type_list_()
  , _common_type_list_cached_byte_size_(){}
struct TestCommonInfoListDefaultTypeInternal {
  constexpr TestCommonInfoListDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~TestCommonInfoListDefaultTypeInternal() {}
  union {
    TestCommonInfoList _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT TestCommonInfoListDefaultTypeInternal _TestCommonInfoList_default_instance_;
}  // namespace TestMsg
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_TestMsgStruct_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_TestMsgStruct_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_TestMsgStruct_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_TestMsgStruct_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_float_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_double_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_string_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_int32_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_sint32_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_uint32_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_int64_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_sint64_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfo, test_uint64_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfoList, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfoList, common_info_list_),
  PROTOBUF_FIELD_OFFSET(::TestMsg::TestCommonInfoList, common_type_list_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::TestMsg::TestCommonInfo)},
  { 14, -1, sizeof(::TestMsg::TestCommonInfoList)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::TestMsg::_TestCommonInfo_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::TestMsg::_TestCommonInfoList_default_instance_),
};

const char descriptor_table_protodef_TestMsgStruct_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\023TestMsgStruct.proto\022\007TestMsg\032\023TestMsgD"
  "efine.proto\"\312\001\n\016TestCommonInfo\022\022\n\ntest_f"
  "loat\030\001 \001(\002\022\023\n\013test_double\030\002 \001(\001\022\023\n\013test_"
  "string\030\003 \001(\014\022\022\n\ntest_int32\030\004 \001(\005\022\023\n\013test"
  "_sint32\030\005 \001(\021\022\023\n\013test_uint32\030\006 \001(\r\022\022\n\nte"
  "st_int64\030\007 \001(\003\022\023\n\013test_sint64\030\010 \001(\022\022\023\n\013t"
  "est_uint64\030\t \001(\004\"z\n\022TestCommonInfoList\0221"
  "\n\020common_info_list\030\001 \003(\0132\027.TestMsg.TestC"
  "ommonInfo\0221\n\020common_type_list\030\002 \003(\0162\027.Te"
  "stMsg.TestCommonTypeb\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_TestMsgStruct_2eproto_deps[1] = {
  &::descriptor_table_TestMsgDefine_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_TestMsgStruct_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_TestMsgStruct_2eproto = {
  false, false, 388, descriptor_table_protodef_TestMsgStruct_2eproto, "TestMsgStruct.proto", 
  &descriptor_table_TestMsgStruct_2eproto_once, descriptor_table_TestMsgStruct_2eproto_deps, 1, 2,
  schemas, file_default_instances, TableStruct_TestMsgStruct_2eproto::offsets,
  file_level_metadata_TestMsgStruct_2eproto, file_level_enum_descriptors_TestMsgStruct_2eproto, file_level_service_descriptors_TestMsgStruct_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK ::PROTOBUF_NAMESPACE_ID::Metadata
descriptor_table_TestMsgStruct_2eproto_metadata_getter(int index) {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_TestMsgStruct_2eproto);
  return descriptor_table_TestMsgStruct_2eproto.file_level_metadata[index];
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_TestMsgStruct_2eproto(&descriptor_table_TestMsgStruct_2eproto);
namespace TestMsg {

// ===================================================================

class TestCommonInfo::_Internal {
 public:
};

TestCommonInfo::TestCommonInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:TestMsg.TestCommonInfo)
}
TestCommonInfo::TestCommonInfo(const TestCommonInfo& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  test_string_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_test_string().empty()) {
    test_string_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_test_string(), 
      GetArena());
  }
  ::memcpy(&test_double_, &from.test_double_,
    static_cast<size_t>(reinterpret_cast<char*>(&test_uint64_) -
    reinterpret_cast<char*>(&test_double_)) + sizeof(test_uint64_));
  // @@protoc_insertion_point(copy_constructor:TestMsg.TestCommonInfo)
}

void TestCommonInfo::SharedCtor() {
test_string_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&test_double_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&test_uint64_) -
    reinterpret_cast<char*>(&test_double_)) + sizeof(test_uint64_));
}

TestCommonInfo::~TestCommonInfo() {
  // @@protoc_insertion_point(destructor:TestMsg.TestCommonInfo)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void TestCommonInfo::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  test_string_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void TestCommonInfo::ArenaDtor(void* object) {
  TestCommonInfo* _this = reinterpret_cast< TestCommonInfo* >(object);
  (void)_this;
}
void TestCommonInfo::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void TestCommonInfo::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void TestCommonInfo::Clear() {
// @@protoc_insertion_point(message_clear_start:TestMsg.TestCommonInfo)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  test_string_.ClearToEmpty();
  ::memset(&test_double_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&test_uint64_) -
      reinterpret_cast<char*>(&test_double_)) + sizeof(test_uint64_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* TestCommonInfo::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // float test_float = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 13)) {
          test_float_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr);
          ptr += sizeof(float);
        } else goto handle_unusual;
        continue;
      // double test_double = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 17)) {
          test_double_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // bytes test_string = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_test_string();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // int32 test_int32 = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          test_int32_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // sint32 test_sint32 = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          test_sint32_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarintZigZag32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 test_uint32 = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          test_uint32_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // int64 test_int64 = 7;
      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          test_int64_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // sint64 test_sint64 = 8;
      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          test_sint64_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarintZigZag64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint64 test_uint64 = 9;
      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          test_uint64_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* TestCommonInfo::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:TestMsg.TestCommonInfo)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // float test_float = 1;
  if (!(this->test_float() <= 0 && this->test_float() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFloatToArray(1, this->_internal_test_float(), target);
  }

  // double test_double = 2;
  if (!(this->test_double() <= 0 && this->test_double() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(2, this->_internal_test_double(), target);
  }

  // bytes test_string = 3;
  if (this->test_string().size() > 0) {
    target = stream->WriteBytesMaybeAliased(
        3, this->_internal_test_string(), target);
  }

  // int32 test_int32 = 4;
  if (this->test_int32() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(4, this->_internal_test_int32(), target);
  }

  // sint32 test_sint32 = 5;
  if (this->test_sint32() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteSInt32ToArray(5, this->_internal_test_sint32(), target);
  }

  // uint32 test_uint32 = 6;
  if (this->test_uint32() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(6, this->_internal_test_uint32(), target);
  }

  // int64 test_int64 = 7;
  if (this->test_int64() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(7, this->_internal_test_int64(), target);
  }

  // sint64 test_sint64 = 8;
  if (this->test_sint64() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteSInt64ToArray(8, this->_internal_test_sint64(), target);
  }

  // uint64 test_uint64 = 9;
  if (this->test_uint64() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(9, this->_internal_test_uint64(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:TestMsg.TestCommonInfo)
  return target;
}

size_t TestCommonInfo::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:TestMsg.TestCommonInfo)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes test_string = 3;
  if (this->test_string().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_test_string());
  }

  // double test_double = 2;
  if (!(this->test_double() <= 0 && this->test_double() >= 0)) {
    total_size += 1 + 8;
  }

  // float test_float = 1;
  if (!(this->test_float() <= 0 && this->test_float() >= 0)) {
    total_size += 1 + 4;
  }

  // int32 test_int32 = 4;
  if (this->test_int32() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_test_int32());
  }

  // sint32 test_sint32 = 5;
  if (this->test_sint32() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SInt32Size(
        this->_internal_test_sint32());
  }

  // uint32 test_uint32 = 6;
  if (this->test_uint32() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_test_uint32());
  }

  // int64 test_int64 = 7;
  if (this->test_int64() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64Size(
        this->_internal_test_int64());
  }

  // sint64 test_sint64 = 8;
  if (this->test_sint64() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SInt64Size(
        this->_internal_test_sint64());
  }

  // uint64 test_uint64 = 9;
  if (this->test_uint64() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
        this->_internal_test_uint64());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void TestCommonInfo::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:TestMsg.TestCommonInfo)
  GOOGLE_DCHECK_NE(&from, this);
  const TestCommonInfo* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<TestCommonInfo>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:TestMsg.TestCommonInfo)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:TestMsg.TestCommonInfo)
    MergeFrom(*source);
  }
}

void TestCommonInfo::MergeFrom(const TestCommonInfo& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:TestMsg.TestCommonInfo)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.test_string().size() > 0) {
    _internal_set_test_string(from._internal_test_string());
  }
  if (!(from.test_double() <= 0 && from.test_double() >= 0)) {
    _internal_set_test_double(from._internal_test_double());
  }
  if (!(from.test_float() <= 0 && from.test_float() >= 0)) {
    _internal_set_test_float(from._internal_test_float());
  }
  if (from.test_int32() != 0) {
    _internal_set_test_int32(from._internal_test_int32());
  }
  if (from.test_sint32() != 0) {
    _internal_set_test_sint32(from._internal_test_sint32());
  }
  if (from.test_uint32() != 0) {
    _internal_set_test_uint32(from._internal_test_uint32());
  }
  if (from.test_int64() != 0) {
    _internal_set_test_int64(from._internal_test_int64());
  }
  if (from.test_sint64() != 0) {
    _internal_set_test_sint64(from._internal_test_sint64());
  }
  if (from.test_uint64() != 0) {
    _internal_set_test_uint64(from._internal_test_uint64());
  }
}

void TestCommonInfo::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:TestMsg.TestCommonInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TestCommonInfo::CopyFrom(const TestCommonInfo& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:TestMsg.TestCommonInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TestCommonInfo::IsInitialized() const {
  return true;
}

void TestCommonInfo::InternalSwap(TestCommonInfo* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  test_string_.Swap(&other->test_string_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(TestCommonInfo, test_uint64_)
      + sizeof(TestCommonInfo::test_uint64_)
      - PROTOBUF_FIELD_OFFSET(TestCommonInfo, test_double_)>(
          reinterpret_cast<char*>(&test_double_),
          reinterpret_cast<char*>(&other->test_double_));
}

::PROTOBUF_NAMESPACE_ID::Metadata TestCommonInfo::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

class TestCommonInfoList::_Internal {
 public:
};

TestCommonInfoList::TestCommonInfoList(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena),
  common_info_list_(arena),
  common_type_list_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:TestMsg.TestCommonInfoList)
}
TestCommonInfoList::TestCommonInfoList(const TestCommonInfoList& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      common_info_list_(from.common_info_list_),
      common_type_list_(from.common_type_list_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:TestMsg.TestCommonInfoList)
}

void TestCommonInfoList::SharedCtor() {
}

TestCommonInfoList::~TestCommonInfoList() {
  // @@protoc_insertion_point(destructor:TestMsg.TestCommonInfoList)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void TestCommonInfoList::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void TestCommonInfoList::ArenaDtor(void* object) {
  TestCommonInfoList* _this = reinterpret_cast< TestCommonInfoList* >(object);
  (void)_this;
}
void TestCommonInfoList::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void TestCommonInfoList::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void TestCommonInfoList::Clear() {
// @@protoc_insertion_point(message_clear_start:TestMsg.TestCommonInfoList)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  common_info_list_.Clear();
  common_type_list_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* TestCommonInfoList::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // repeated .TestMsg.TestCommonInfo common_info_list = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_common_info_list(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<10>(ptr));
        } else goto handle_unusual;
        continue;
      // repeated .TestMsg.TestCommonType common_type_list = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedEnumParser(_internal_mutable_common_type_list(), ptr, ctx);
          CHK_(ptr);
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_add_common_type_list(static_cast<::TestMsg::TestCommonType>(val));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* TestCommonInfoList::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:TestMsg.TestCommonInfoList)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated .TestMsg.TestCommonInfo common_info_list = 1;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_common_info_list_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(1, this->_internal_common_info_list(i), target, stream);
  }

  // repeated .TestMsg.TestCommonType common_type_list = 2;
  {
    int byte_size = _common_type_list_cached_byte_size_.load(std::memory_order_relaxed);
    if (byte_size > 0) {
      target = stream->WriteEnumPacked(
          2, common_type_list_, byte_size, target);
    }
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:TestMsg.TestCommonInfoList)
  return target;
}

size_t TestCommonInfoList::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:TestMsg.TestCommonInfoList)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .TestMsg.TestCommonInfo common_info_list = 1;
  total_size += 1UL * this->_internal_common_info_list_size();
  for (const auto& msg : this->common_info_list_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  // repeated .TestMsg.TestCommonType common_type_list = 2;
  {
    size_t data_size = 0;
    unsigned int count = static_cast<unsigned int>(this->_internal_common_type_list_size());for (unsigned int i = 0; i < count; i++) {
      data_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(
        this->_internal_common_type_list(static_cast<int>(i)));
    }
    if (data_size > 0) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
            static_cast<::PROTOBUF_NAMESPACE_ID::int32>(data_size));
    }
    int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(data_size);
    _common_type_list_cached_byte_size_.store(cached_size,
                                    std::memory_order_relaxed);
    total_size += data_size;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void TestCommonInfoList::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:TestMsg.TestCommonInfoList)
  GOOGLE_DCHECK_NE(&from, this);
  const TestCommonInfoList* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<TestCommonInfoList>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:TestMsg.TestCommonInfoList)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:TestMsg.TestCommonInfoList)
    MergeFrom(*source);
  }
}

void TestCommonInfoList::MergeFrom(const TestCommonInfoList& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:TestMsg.TestCommonInfoList)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  common_info_list_.MergeFrom(from.common_info_list_);
  common_type_list_.MergeFrom(from.common_type_list_);
}

void TestCommonInfoList::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:TestMsg.TestCommonInfoList)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TestCommonInfoList::CopyFrom(const TestCommonInfoList& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:TestMsg.TestCommonInfoList)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TestCommonInfoList::IsInitialized() const {
  return true;
}

void TestCommonInfoList::InternalSwap(TestCommonInfoList* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  common_info_list_.InternalSwap(&other->common_info_list_);
  common_type_list_.InternalSwap(&other->common_type_list_);
}

::PROTOBUF_NAMESPACE_ID::Metadata TestCommonInfoList::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace TestMsg
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::TestMsg::TestCommonInfo* Arena::CreateMaybeMessage< ::TestMsg::TestCommonInfo >(Arena* arena) {
  return Arena::CreateMessageInternal< ::TestMsg::TestCommonInfo >(arena);
}
template<> PROTOBUF_NOINLINE ::TestMsg::TestCommonInfoList* Arena::CreateMaybeMessage< ::TestMsg::TestCommonInfoList >(Arena* arena) {
  return Arena::CreateMessageInternal< ::TestMsg::TestCommonInfoList >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
