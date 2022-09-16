// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: TestMsgStruct.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_TestMsgStruct_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_TestMsgStruct_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3015000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3015008 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "TestMsgDefine.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_TestMsgStruct_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_TestMsgStruct_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_TestMsgStruct_2eproto;
::PROTOBUF_NAMESPACE_ID::Metadata descriptor_table_TestMsgStruct_2eproto_metadata_getter(int index);
namespace TestMsg {
class CommonInfo;
struct CommonInfoDefaultTypeInternal;
extern CommonInfoDefaultTypeInternal _CommonInfo_default_instance_;
class CommonInfoList;
struct CommonInfoListDefaultTypeInternal;
extern CommonInfoListDefaultTypeInternal _CommonInfoList_default_instance_;
}  // namespace TestMsg
PROTOBUF_NAMESPACE_OPEN
template<> ::TestMsg::CommonInfo* Arena::CreateMaybeMessage<::TestMsg::CommonInfo>(Arena*);
template<> ::TestMsg::CommonInfoList* Arena::CreateMaybeMessage<::TestMsg::CommonInfoList>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace TestMsg {

// ===================================================================

class CommonInfo PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:TestMsg.CommonInfo) */ {
 public:
  inline CommonInfo() : CommonInfo(nullptr) {}
  virtual ~CommonInfo();
  explicit constexpr CommonInfo(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  CommonInfo(const CommonInfo& from);
  CommonInfo(CommonInfo&& from) noexcept
    : CommonInfo() {
    *this = ::std::move(from);
  }

  inline CommonInfo& operator=(const CommonInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline CommonInfo& operator=(CommonInfo&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const CommonInfo& default_instance() {
    return *internal_default_instance();
  }
  static inline const CommonInfo* internal_default_instance() {
    return reinterpret_cast<const CommonInfo*>(
               &_CommonInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(CommonInfo& a, CommonInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(CommonInfo* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(CommonInfo* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline CommonInfo* New() const final {
    return CreateMaybeMessage<CommonInfo>(nullptr);
  }

  CommonInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<CommonInfo>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const CommonInfo& from);
  void MergeFrom(const CommonInfo& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(CommonInfo* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "TestMsg.CommonInfo";
  }
  protected:
  explicit CommonInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_TestMsgStruct_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTestStringFieldNumber = 3,
    kTestDoubleFieldNumber = 2,
    kTestFloatFieldNumber = 1,
    kTestInt32FieldNumber = 4,
    kTestSint32FieldNumber = 5,
    kTestUint32FieldNumber = 6,
    kTestInt64FieldNumber = 7,
    kTestSint64FieldNumber = 8,
    kTestUint64FieldNumber = 9,
  };
  // bytes test_string = 3;
  void clear_test_string();
  const std::string& test_string() const;
  void set_test_string(const std::string& value);
  void set_test_string(std::string&& value);
  void set_test_string(const char* value);
  void set_test_string(const void* value, size_t size);
  std::string* mutable_test_string();
  std::string* release_test_string();
  void set_allocated_test_string(std::string* test_string);
  private:
  const std::string& _internal_test_string() const;
  void _internal_set_test_string(const std::string& value);
  std::string* _internal_mutable_test_string();
  public:

  // double test_double = 2;
  void clear_test_double();
  double test_double() const;
  void set_test_double(double value);
  private:
  double _internal_test_double() const;
  void _internal_set_test_double(double value);
  public:

  // float test_float = 1;
  void clear_test_float();
  float test_float() const;
  void set_test_float(float value);
  private:
  float _internal_test_float() const;
  void _internal_set_test_float(float value);
  public:

  // int32 test_int32 = 4;
  void clear_test_int32();
  ::PROTOBUF_NAMESPACE_ID::int32 test_int32() const;
  void set_test_int32(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_test_int32() const;
  void _internal_set_test_int32(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // sint32 test_sint32 = 5;
  void clear_test_sint32();
  ::PROTOBUF_NAMESPACE_ID::int32 test_sint32() const;
  void set_test_sint32(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_test_sint32() const;
  void _internal_set_test_sint32(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // uint32 test_uint32 = 6;
  void clear_test_uint32();
  ::PROTOBUF_NAMESPACE_ID::uint32 test_uint32() const;
  void set_test_uint32(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_test_uint32() const;
  void _internal_set_test_uint32(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // int64 test_int64 = 7;
  void clear_test_int64();
  ::PROTOBUF_NAMESPACE_ID::int64 test_int64() const;
  void set_test_int64(::PROTOBUF_NAMESPACE_ID::int64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int64 _internal_test_int64() const;
  void _internal_set_test_int64(::PROTOBUF_NAMESPACE_ID::int64 value);
  public:

  // sint64 test_sint64 = 8;
  void clear_test_sint64();
  ::PROTOBUF_NAMESPACE_ID::int64 test_sint64() const;
  void set_test_sint64(::PROTOBUF_NAMESPACE_ID::int64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int64 _internal_test_sint64() const;
  void _internal_set_test_sint64(::PROTOBUF_NAMESPACE_ID::int64 value);
  public:

  // uint64 test_uint64 = 9;
  void clear_test_uint64();
  ::PROTOBUF_NAMESPACE_ID::uint64 test_uint64() const;
  void set_test_uint64(::PROTOBUF_NAMESPACE_ID::uint64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint64 _internal_test_uint64() const;
  void _internal_set_test_uint64(::PROTOBUF_NAMESPACE_ID::uint64 value);
  public:

  // @@protoc_insertion_point(class_scope:TestMsg.CommonInfo)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr test_string_;
  double test_double_;
  float test_float_;
  ::PROTOBUF_NAMESPACE_ID::int32 test_int32_;
  ::PROTOBUF_NAMESPACE_ID::int32 test_sint32_;
  ::PROTOBUF_NAMESPACE_ID::uint32 test_uint32_;
  ::PROTOBUF_NAMESPACE_ID::int64 test_int64_;
  ::PROTOBUF_NAMESPACE_ID::int64 test_sint64_;
  ::PROTOBUF_NAMESPACE_ID::uint64 test_uint64_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_TestMsgStruct_2eproto;
};
// -------------------------------------------------------------------

class CommonInfoList PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:TestMsg.CommonInfoList) */ {
 public:
  inline CommonInfoList() : CommonInfoList(nullptr) {}
  virtual ~CommonInfoList();
  explicit constexpr CommonInfoList(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  CommonInfoList(const CommonInfoList& from);
  CommonInfoList(CommonInfoList&& from) noexcept
    : CommonInfoList() {
    *this = ::std::move(from);
  }

  inline CommonInfoList& operator=(const CommonInfoList& from) {
    CopyFrom(from);
    return *this;
  }
  inline CommonInfoList& operator=(CommonInfoList&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const CommonInfoList& default_instance() {
    return *internal_default_instance();
  }
  static inline const CommonInfoList* internal_default_instance() {
    return reinterpret_cast<const CommonInfoList*>(
               &_CommonInfoList_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(CommonInfoList& a, CommonInfoList& b) {
    a.Swap(&b);
  }
  inline void Swap(CommonInfoList* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(CommonInfoList* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline CommonInfoList* New() const final {
    return CreateMaybeMessage<CommonInfoList>(nullptr);
  }

  CommonInfoList* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<CommonInfoList>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const CommonInfoList& from);
  void MergeFrom(const CommonInfoList& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(CommonInfoList* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "TestMsg.CommonInfoList";
  }
  protected:
  explicit CommonInfoList(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_TestMsgStruct_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kCommonInfoListFieldNumber = 1,
    kCommonTypeListFieldNumber = 2,
  };
  // repeated .TestMsg.CommonInfo common_info_list = 1;
  int common_info_list_size() const;
  private:
  int _internal_common_info_list_size() const;
  public:
  void clear_common_info_list();
  ::TestMsg::CommonInfo* mutable_common_info_list(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::TestMsg::CommonInfo >*
      mutable_common_info_list();
  private:
  const ::TestMsg::CommonInfo& _internal_common_info_list(int index) const;
  ::TestMsg::CommonInfo* _internal_add_common_info_list();
  public:
  const ::TestMsg::CommonInfo& common_info_list(int index) const;
  ::TestMsg::CommonInfo* add_common_info_list();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::TestMsg::CommonInfo >&
      common_info_list() const;

  // repeated .TestMsg.CommonType common_type_list = 2;
  int common_type_list_size() const;
  private:
  int _internal_common_type_list_size() const;
  public:
  void clear_common_type_list();
  private:
  ::TestMsg::CommonType _internal_common_type_list(int index) const;
  void _internal_add_common_type_list(::TestMsg::CommonType value);
  ::PROTOBUF_NAMESPACE_ID::RepeatedField<int>* _internal_mutable_common_type_list();
  public:
  ::TestMsg::CommonType common_type_list(int index) const;
  void set_common_type_list(int index, ::TestMsg::CommonType value);
  void add_common_type_list(::TestMsg::CommonType value);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField<int>& common_type_list() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField<int>* mutable_common_type_list();

  // @@protoc_insertion_point(class_scope:TestMsg.CommonInfoList)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::TestMsg::CommonInfo > common_info_list_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField<int> common_type_list_;
  mutable std::atomic<int> _common_type_list_cached_byte_size_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_TestMsgStruct_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// CommonInfo

// float test_float = 1;
inline void CommonInfo::clear_test_float() {
  test_float_ = 0;
}
inline float CommonInfo::_internal_test_float() const {
  return test_float_;
}
inline float CommonInfo::test_float() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_float)
  return _internal_test_float();
}
inline void CommonInfo::_internal_set_test_float(float value) {
  
  test_float_ = value;
}
inline void CommonInfo::set_test_float(float value) {
  _internal_set_test_float(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_float)
}

// double test_double = 2;
inline void CommonInfo::clear_test_double() {
  test_double_ = 0;
}
inline double CommonInfo::_internal_test_double() const {
  return test_double_;
}
inline double CommonInfo::test_double() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_double)
  return _internal_test_double();
}
inline void CommonInfo::_internal_set_test_double(double value) {
  
  test_double_ = value;
}
inline void CommonInfo::set_test_double(double value) {
  _internal_set_test_double(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_double)
}

// bytes test_string = 3;
inline void CommonInfo::clear_test_string() {
  test_string_.ClearToEmpty();
}
inline const std::string& CommonInfo::test_string() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_string)
  return _internal_test_string();
}
inline void CommonInfo::set_test_string(const std::string& value) {
  _internal_set_test_string(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_string)
}
inline std::string* CommonInfo::mutable_test_string() {
  // @@protoc_insertion_point(field_mutable:TestMsg.CommonInfo.test_string)
  return _internal_mutable_test_string();
}
inline const std::string& CommonInfo::_internal_test_string() const {
  return test_string_.Get();
}
inline void CommonInfo::_internal_set_test_string(const std::string& value) {
  
  test_string_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void CommonInfo::set_test_string(std::string&& value) {
  
  test_string_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:TestMsg.CommonInfo.test_string)
}
inline void CommonInfo::set_test_string(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  test_string_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:TestMsg.CommonInfo.test_string)
}
inline void CommonInfo::set_test_string(const void* value,
    size_t size) {
  
  test_string_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:TestMsg.CommonInfo.test_string)
}
inline std::string* CommonInfo::_internal_mutable_test_string() {
  
  return test_string_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* CommonInfo::release_test_string() {
  // @@protoc_insertion_point(field_release:TestMsg.CommonInfo.test_string)
  return test_string_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void CommonInfo::set_allocated_test_string(std::string* test_string) {
  if (test_string != nullptr) {
    
  } else {
    
  }
  test_string_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), test_string,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:TestMsg.CommonInfo.test_string)
}

// int32 test_int32 = 4;
inline void CommonInfo::clear_test_int32() {
  test_int32_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 CommonInfo::_internal_test_int32() const {
  return test_int32_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 CommonInfo::test_int32() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_int32)
  return _internal_test_int32();
}
inline void CommonInfo::_internal_set_test_int32(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  test_int32_ = value;
}
inline void CommonInfo::set_test_int32(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_test_int32(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_int32)
}

// sint32 test_sint32 = 5;
inline void CommonInfo::clear_test_sint32() {
  test_sint32_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 CommonInfo::_internal_test_sint32() const {
  return test_sint32_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 CommonInfo::test_sint32() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_sint32)
  return _internal_test_sint32();
}
inline void CommonInfo::_internal_set_test_sint32(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  test_sint32_ = value;
}
inline void CommonInfo::set_test_sint32(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_test_sint32(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_sint32)
}

// uint32 test_uint32 = 6;
inline void CommonInfo::clear_test_uint32() {
  test_uint32_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 CommonInfo::_internal_test_uint32() const {
  return test_uint32_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 CommonInfo::test_uint32() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_uint32)
  return _internal_test_uint32();
}
inline void CommonInfo::_internal_set_test_uint32(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  test_uint32_ = value;
}
inline void CommonInfo::set_test_uint32(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_test_uint32(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_uint32)
}

// int64 test_int64 = 7;
inline void CommonInfo::clear_test_int64() {
  test_int64_ = PROTOBUF_LONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::int64 CommonInfo::_internal_test_int64() const {
  return test_int64_;
}
inline ::PROTOBUF_NAMESPACE_ID::int64 CommonInfo::test_int64() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_int64)
  return _internal_test_int64();
}
inline void CommonInfo::_internal_set_test_int64(::PROTOBUF_NAMESPACE_ID::int64 value) {
  
  test_int64_ = value;
}
inline void CommonInfo::set_test_int64(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _internal_set_test_int64(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_int64)
}

// sint64 test_sint64 = 8;
inline void CommonInfo::clear_test_sint64() {
  test_sint64_ = PROTOBUF_LONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::int64 CommonInfo::_internal_test_sint64() const {
  return test_sint64_;
}
inline ::PROTOBUF_NAMESPACE_ID::int64 CommonInfo::test_sint64() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_sint64)
  return _internal_test_sint64();
}
inline void CommonInfo::_internal_set_test_sint64(::PROTOBUF_NAMESPACE_ID::int64 value) {
  
  test_sint64_ = value;
}
inline void CommonInfo::set_test_sint64(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _internal_set_test_sint64(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_sint64)
}

// uint64 test_uint64 = 9;
inline void CommonInfo::clear_test_uint64() {
  test_uint64_ = PROTOBUF_ULONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 CommonInfo::_internal_test_uint64() const {
  return test_uint64_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 CommonInfo::test_uint64() const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfo.test_uint64)
  return _internal_test_uint64();
}
inline void CommonInfo::_internal_set_test_uint64(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  
  test_uint64_ = value;
}
inline void CommonInfo::set_test_uint64(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  _internal_set_test_uint64(value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfo.test_uint64)
}

// -------------------------------------------------------------------

// CommonInfoList

// repeated .TestMsg.CommonInfo common_info_list = 1;
inline int CommonInfoList::_internal_common_info_list_size() const {
  return common_info_list_.size();
}
inline int CommonInfoList::common_info_list_size() const {
  return _internal_common_info_list_size();
}
inline void CommonInfoList::clear_common_info_list() {
  common_info_list_.Clear();
}
inline ::TestMsg::CommonInfo* CommonInfoList::mutable_common_info_list(int index) {
  // @@protoc_insertion_point(field_mutable:TestMsg.CommonInfoList.common_info_list)
  return common_info_list_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::TestMsg::CommonInfo >*
CommonInfoList::mutable_common_info_list() {
  // @@protoc_insertion_point(field_mutable_list:TestMsg.CommonInfoList.common_info_list)
  return &common_info_list_;
}
inline const ::TestMsg::CommonInfo& CommonInfoList::_internal_common_info_list(int index) const {
  return common_info_list_.Get(index);
}
inline const ::TestMsg::CommonInfo& CommonInfoList::common_info_list(int index) const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfoList.common_info_list)
  return _internal_common_info_list(index);
}
inline ::TestMsg::CommonInfo* CommonInfoList::_internal_add_common_info_list() {
  return common_info_list_.Add();
}
inline ::TestMsg::CommonInfo* CommonInfoList::add_common_info_list() {
  // @@protoc_insertion_point(field_add:TestMsg.CommonInfoList.common_info_list)
  return _internal_add_common_info_list();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::TestMsg::CommonInfo >&
CommonInfoList::common_info_list() const {
  // @@protoc_insertion_point(field_list:TestMsg.CommonInfoList.common_info_list)
  return common_info_list_;
}

// repeated .TestMsg.CommonType common_type_list = 2;
inline int CommonInfoList::_internal_common_type_list_size() const {
  return common_type_list_.size();
}
inline int CommonInfoList::common_type_list_size() const {
  return _internal_common_type_list_size();
}
inline void CommonInfoList::clear_common_type_list() {
  common_type_list_.Clear();
}
inline ::TestMsg::CommonType CommonInfoList::_internal_common_type_list(int index) const {
  return static_cast< ::TestMsg::CommonType >(common_type_list_.Get(index));
}
inline ::TestMsg::CommonType CommonInfoList::common_type_list(int index) const {
  // @@protoc_insertion_point(field_get:TestMsg.CommonInfoList.common_type_list)
  return _internal_common_type_list(index);
}
inline void CommonInfoList::set_common_type_list(int index, ::TestMsg::CommonType value) {
  common_type_list_.Set(index, value);
  // @@protoc_insertion_point(field_set:TestMsg.CommonInfoList.common_type_list)
}
inline void CommonInfoList::_internal_add_common_type_list(::TestMsg::CommonType value) {
  common_type_list_.Add(value);
}
inline void CommonInfoList::add_common_type_list(::TestMsg::CommonType value) {
  // @@protoc_insertion_point(field_add:TestMsg.CommonInfoList.common_type_list)
  _internal_add_common_type_list(value);
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField<int>&
CommonInfoList::common_type_list() const {
  // @@protoc_insertion_point(field_list:TestMsg.CommonInfoList.common_type_list)
  return common_type_list_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField<int>*
CommonInfoList::_internal_mutable_common_type_list() {
  return &common_type_list_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField<int>*
CommonInfoList::mutable_common_type_list() {
  // @@protoc_insertion_point(field_mutable_list:TestMsg.CommonInfoList.common_type_list)
  return _internal_mutable_common_type_list();
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace TestMsg

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_TestMsgStruct_2eproto
