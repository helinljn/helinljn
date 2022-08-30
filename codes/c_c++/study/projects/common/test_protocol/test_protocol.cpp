#include "test_protocol.h"

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/color.h"

#include "TestMsgDefine.pb.h"
#include "TestMsgStruct.pb.h"

class CommonInfo
{
public:
    bool        test_bool;
    float       test_float;
    double      test_double;
    std::string test_string;
    int32_t     test_int32;
    int32_t     test_sint32;
    uint32_t    test_uint32;
    int64_t     test_int64;
    int64_t     test_sint64;
    uint64_t    test_uint64;
};

bool operator ==(const CommonInfo& cinfo, const TestMsg::CommonInfo& pinfo)
{
    return cinfo.test_bool   == pinfo.test_bool()
        && cinfo.test_float  == pinfo.test_float()
        && cinfo.test_double == pinfo.test_double()
        && cinfo.test_string == pinfo.test_string()
        && cinfo.test_int32  == pinfo.test_int32()
        && cinfo.test_sint32 == pinfo.test_sint32()
        && cinfo.test_uint32 == pinfo.test_uint32()
        && cinfo.test_int64 == pinfo.test_int64()
        && cinfo.test_sint64 == pinfo.test_sint64()
        && cinfo.test_uint64 == pinfo.test_uint64();
}

bool operator ==(const TestMsg::CommonInfo& pinfo, const CommonInfo& cinfo)
{
    return cinfo == pinfo;
}

void test_protocol_enum(void)
{
    // CommonType_Name
    const std::string& str_bool     = TestMsg::CommonType_Name(TestMsg::CT_BOOL);
    const std::string& str_bytes    = TestMsg::CommonType_Name(TestMsg::CT_BYTES);
    const std::string& str_double   = TestMsg::CommonType_Name(TestMsg::CT_DOUBLE);
    const std::string& str_float    = TestMsg::CommonType_Name(TestMsg::CT_FLOAT);
    const std::string& str_int32    = TestMsg::CommonType_Name(TestMsg::CT_INT32);
    const std::string& str_sint32   = TestMsg::CommonType_Name(TestMsg::CT_SINT32);
    const std::string& str_uint32   = TestMsg::CommonType_Name(TestMsg::CT_UINT32);
    const std::string& str_int64    = TestMsg::CommonType_Name(TestMsg::CT_INT64);
    const std::string& str_sint64   = TestMsg::CommonType_Name(TestMsg::CT_SINT64);
    const std::string& str_uint64   = TestMsg::CommonType_Name(TestMsg::CT_UINT64);
    const std::string& str_default1 = TestMsg::CommonType_Name(TestMsg::CommonType{});
    const std::string& str_default2 = TestMsg::CommonType_Name(TestMsg::CommonType(0));
    const std::string& str_null1    = TestMsg::CommonType_Name(TestMsg::CommonType(100));
    const std::string& str_null2    = TestMsg::CommonType_Name(TestMsg::CommonType(-1));

    abort_assert(str_bool     == "CT_BOOL");
    abort_assert(str_bytes    == "CT_BYTES");
    abort_assert(str_double   == "CT_DOUBLE");
    abort_assert(str_float    == "CT_FLOAT");
    abort_assert(str_int32    == "CT_INT32");
    abort_assert(str_sint32   == "CT_SINT32");
    abort_assert(str_uint32   == "CT_UINT32");
    abort_assert(str_int64    == "CT_INT64");
    abort_assert(str_sint64   == "CT_SINT64");
    abort_assert(str_uint64   == "CT_UINT64");
    abort_assert(str_default1 == "CT_BOOL");
    abort_assert(str_default2 == "CT_BOOL");
    abort_assert(str_null1.empty());
    abort_assert(str_null2.empty());

    // CommonType_Parse
    TestMsg::CommonType val{};
    abort_assert(TestMsg::CT_BOOL == val);

    abort_assert(TestMsg::CommonType_Parse(str_bool, &val));
    abort_assert(TestMsg::CT_BOOL == val);

    abort_assert(TestMsg::CommonType_Parse(str_bytes, &val));
    abort_assert(TestMsg::CT_BYTES == val);

    abort_assert(TestMsg::CommonType_Parse(str_double, &val));
    abort_assert(TestMsg::CT_DOUBLE == val);

    abort_assert(TestMsg::CommonType_Parse(str_float, &val));
    abort_assert(TestMsg::CT_FLOAT == val);

    abort_assert(TestMsg::CommonType_Parse(str_int32, &val));
    abort_assert(TestMsg::CT_INT32 == val);

    abort_assert(TestMsg::CommonType_Parse(str_sint32, &val));
    abort_assert(TestMsg::CT_SINT32 == val);

    abort_assert(TestMsg::CommonType_Parse(str_uint32, &val));
    abort_assert(TestMsg::CT_UINT32 == val);

    abort_assert(TestMsg::CommonType_Parse(str_int64, &val));
    abort_assert(TestMsg::CT_INT64 == val);

    abort_assert(TestMsg::CommonType_Parse(str_sint64, &val));
    abort_assert(TestMsg::CT_SINT64 == val);

    abort_assert(TestMsg::CommonType_Parse(str_uint64, &val));
    abort_assert(TestMsg::CT_UINT64 == val);

    abort_assert(TestMsg::CommonType_Parse(str_default1, &val));
    abort_assert(TestMsg::CT_BOOL == val);

    abort_assert(TestMsg::CommonType_Parse(str_default2, &val));
    abort_assert(TestMsg::CT_BOOL == val);

    abort_assert(!TestMsg::CommonType_Parse(str_null1, &val));
    abort_assert(!TestMsg::CommonType_Parse(str_null2, &val));
}

void test_protocol_serialization(void)
{
    CommonInfo info{ true, 1.23f, 4.5678, "CommonInfo", 123456, -123456, 123456, 567890, -567890, 567890 };

    TestMsg::CommonInfo test_info;
    test_info.set_test_bool(info.test_bool);
    test_info.set_test_float(info.test_float);
    test_info.set_test_double(info.test_double);
    test_info.set_test_string(info.test_string);
    test_info.set_test_int32(info.test_int32);
    test_info.set_test_sint32(info.test_sint32);
    test_info.set_test_uint32(info.test_uint32);
    test_info.set_test_int64(info.test_int64);
    test_info.set_test_sint64(info.test_sint64);
    test_info.set_test_uint64(info.test_uint64);

    // ByteSizeLong
    size_t test_info_size = test_info.ByteSizeLong();
    fmt::print("test_info.ByteSizeLong={}\n", test_info_size);

    // DebugString
    fmt::print("---- test_info.DebugString() start ----\n");
    fmt::print("{}", test_info.DebugString());
    fmt::print("---- test_info.DebugString() end ----\n");

    // SerializeToString & ParseFromString
    std::string info_to_string;
    abort_assert(test_info.SerializeToString(&info_to_string));
    abort_assert(test_info_size == info_to_string.size());
    fmt::print("test_info.ByteSizeLong={}, test_info.SerializeToString={}\n", test_info.ByteSizeLong(), info_to_string);

    TestMsg::CommonInfo string_to_info;
    abort_assert(string_to_info.ParseFromString(info_to_string));
    abort_assert(string_to_info == info);
    abort_assert(string_to_info.ByteSizeLong() == test_info_size);

    // SerializeToArray & ParseFromArray
    char info_to_array[128] = {0};
    abort_assert(test_info.SerializeToArray(info_to_array, sizeof(info_to_array)));
    abort_assert(test_info_size == strlen(info_to_array));
    fmt::print("test_info.ByteSizeLong={}, test_info.SerializeToArray={}\n", test_info.ByteSizeLong(), info_to_array);

    TestMsg::CommonInfo array_to_info;
    abort_assert(array_to_info.ParseFromArray(info_to_array, static_cast<int>(strlen(info_to_array))));
    abort_assert(array_to_info == info);
    abort_assert(array_to_info.ByteSizeLong() == test_info_size);

    fmt::print(fmt::fg(fmt::color::green), "{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __func__)));
}

void test_protocol_all(void)
{
    test_protocol_enum();
    test_protocol_serialization();
}