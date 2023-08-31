#include "gtest/gtest.h"
#include "pbmsg/TestMsgDefine.pb.h"
#include "pbmsg/TestMsgStruct.pb.h"

#include <string>
#include <vector>
#include <utility>

// Unnamed namespace for internal linkage
namespace {

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

bool operator ==(const CommonInfo& cinfo, const PBMsg::TestCommonInfo& pinfo)
{
    return cinfo.test_bool   == pinfo.test_bool()
        && cinfo.test_float  == pinfo.test_float()
        && cinfo.test_double == pinfo.test_double()
        && cinfo.test_string == pinfo.test_string()
        && cinfo.test_int32  == pinfo.test_int32()
        && cinfo.test_sint32 == pinfo.test_sint32()
        && cinfo.test_uint32 == pinfo.test_uint32()
        && cinfo.test_int64  == pinfo.test_int64()
        && cinfo.test_sint64 == pinfo.test_sint64()
        && cinfo.test_uint64 == pinfo.test_uint64();
}

bool operator ==(const PBMsg::TestCommonInfo& pinfo, const CommonInfo& cinfo)
{
    return cinfo == pinfo;
}

} // unnamed namespace

GTEST_TEST(ProtobufTest, EnumName)
{
    // CommonType_Name
    const std::string& str_bool     = PBMsg::TestCommonType_Name(PBMsg::CT_BOOL);
    const std::string& str_float    = PBMsg::TestCommonType_Name(PBMsg::CT_FLOAT);
    const std::string& str_double   = PBMsg::TestCommonType_Name(PBMsg::CT_DOUBLE);
    const std::string& str_bytes    = PBMsg::TestCommonType_Name(PBMsg::CT_BYTES);
    const std::string& str_int32    = PBMsg::TestCommonType_Name(PBMsg::CT_INT32);
    const std::string& str_sint32   = PBMsg::TestCommonType_Name(PBMsg::CT_SINT32);
    const std::string& str_uint32   = PBMsg::TestCommonType_Name(PBMsg::CT_UINT32);
    const std::string& str_int64    = PBMsg::TestCommonType_Name(PBMsg::CT_INT64);
    const std::string& str_sint64   = PBMsg::TestCommonType_Name(PBMsg::CT_SINT64);
    const std::string& str_uint64   = PBMsg::TestCommonType_Name(PBMsg::CT_UINT64);
    const std::string& str_default1 = PBMsg::TestCommonType_Name(PBMsg::TestCommonType{});
    const std::string& str_default2 = PBMsg::TestCommonType_Name(PBMsg::TestCommonType(0));
    const std::string& str_null1    = PBMsg::TestCommonType_Name(PBMsg::TestCommonType(100));
    const std::string& str_null2    = PBMsg::TestCommonType_Name(PBMsg::TestCommonType(-1));

    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_BOOL));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_FLOAT));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_DOUBLE));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_BYTES));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_INT32));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_SINT32));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_UINT32));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_INT64));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_SINT64));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::CT_UINT64));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::TestCommonType{}));
    ASSERT_TRUE(PBMsg::TestCommonType_IsValid(PBMsg::TestCommonType(0)));

    ASSERT_FALSE(PBMsg::TestCommonType_IsValid(PBMsg::TestCommonType(100)));
    ASSERT_FALSE(PBMsg::TestCommonType_IsValid(PBMsg::TestCommonType(-1)));

    ASSERT_TRUE(str_bool     == "CT_BOOL");
    ASSERT_TRUE(str_float    == "CT_FLOAT");
    ASSERT_TRUE(str_double   == "CT_DOUBLE");
    ASSERT_TRUE(str_bytes    == "CT_BYTES");
    ASSERT_TRUE(str_int32    == "CT_INT32");
    ASSERT_TRUE(str_sint32   == "CT_SINT32");
    ASSERT_TRUE(str_uint32   == "CT_UINT32");
    ASSERT_TRUE(str_int64    == "CT_INT64");
    ASSERT_TRUE(str_sint64   == "CT_SINT64");
    ASSERT_TRUE(str_uint64   == "CT_UINT64");
    ASSERT_TRUE(str_default1 == "CT_BOOL");
    ASSERT_TRUE(str_default2 == "CT_BOOL");

    ASSERT_TRUE(str_null1.empty());
    ASSERT_TRUE(str_null2.empty());

    // CommonType_Parse
    PBMsg::TestCommonType val{};
    ASSERT_TRUE(PBMsg::CT_BOOL == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_bool, &val));
    ASSERT_TRUE(PBMsg::CT_BOOL == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_float, &val));
    ASSERT_TRUE(PBMsg::CT_FLOAT == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_double, &val));
    ASSERT_TRUE(PBMsg::CT_DOUBLE == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_bytes, &val));
    ASSERT_TRUE(PBMsg::CT_BYTES == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_int32, &val));
    ASSERT_TRUE(PBMsg::CT_INT32 == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_sint32, &val));
    ASSERT_TRUE(PBMsg::CT_SINT32 == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_uint32, &val));
    ASSERT_TRUE(PBMsg::CT_UINT32 == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_int64, &val));
    ASSERT_TRUE(PBMsg::CT_INT64 == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_sint64, &val));
    ASSERT_TRUE(PBMsg::CT_SINT64 == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_uint64, &val));
    ASSERT_TRUE(PBMsg::CT_UINT64 == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_default1, &val));
    ASSERT_TRUE(PBMsg::CT_BOOL == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(PBMsg::TestCommonType_Parse(str_default2, &val));
    ASSERT_TRUE(PBMsg::CT_BOOL == val && PBMsg::TestCommonType_IsValid(val));

    ASSERT_FALSE(PBMsg::TestCommonType_Parse(str_null1, &val));
    ASSERT_FALSE(PBMsg::TestCommonType_Parse(str_null2, &val));
}

GTEST_TEST(ProtobufTest, Serialization)
{
    CommonInfo info{true, 1.23f, 4.5678, "CommonInfo", 123456, -123456, 123456, 567890, -567890, 567890};

    PBMsg::TestCommonInfo test_info;
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

    // SerializeToString & ParseFromString
    {
        std::string info_to_string;
        ASSERT_TRUE(test_info.SerializeToString(&info_to_string));
        ASSERT_TRUE(test_info_size == info_to_string.size());

        PBMsg::TestCommonInfo string_to_info;
        ASSERT_TRUE(string_to_info.ParseFromString(info_to_string));
        ASSERT_TRUE(string_to_info == info);
        ASSERT_TRUE(string_to_info.ByteSizeLong() == test_info_size);
    }

    // SerializeToArray & ParseFromArray
    {
        char info_to_array[128] = {0};
        ASSERT_TRUE(test_info.SerializeToArray(info_to_array, sizeof(info_to_array)));

        PBMsg::TestCommonInfo array_to_info;
        ASSERT_TRUE(array_to_info.ParseFromArray(info_to_array, static_cast<int>(test_info_size)));
        ASSERT_TRUE(array_to_info == info);
        ASSERT_TRUE(array_to_info.ByteSizeLong() == test_info_size);
    }

    PBMsg::TestCommonInfo test_info_copy = test_info;
    ASSERT_TRUE(test_info_copy == info);

    PBMsg::TestCommonInfo test_info_move = std::move(test_info);
    ASSERT_TRUE(test_info_move == info);

    ASSERT_FALSE(test_info == info);
}

GTEST_TEST(ProtobufTest, SerializationList)
{
    std::vector<CommonInfo> info_list{
        {true, 1.23f, 4.5678, "CommonInfo1", 123456, -123456, 123456, 567890, -567890, 567890},
        {true, 4.56f, 9.0123, "CommonInfo2", 456789, -456789, 456789, 123456, -123456, 123456},
    };

    std::vector<PBMsg::TestCommonType> type_list{
        PBMsg::CT_BOOL,   PBMsg::CT_FLOAT,  PBMsg::CT_DOUBLE, PBMsg::CT_BYTES,  PBMsg::CT_INT32,
        PBMsg::CT_SINT32, PBMsg::CT_UINT32, PBMsg::CT_INT64,  PBMsg::CT_SINT64, PBMsg::CT_UINT64
    };

    // common_info_list_size
    PBMsg::TestCommonInfoList test_info_list;
    ASSERT_TRUE(test_info_list.common_info_list_size() == 0);
    ASSERT_TRUE(test_info_list.common_type_list_size() == 0);

    // add_common_info_list
    PBMsg::TestCommonInfo* temp = test_info_list.add_common_info_list();
    ASSERT_TRUE(temp != nullptr);
    temp->set_test_bool(info_list[0].test_bool);
    temp->set_test_float(info_list[0].test_float);
    temp->set_test_double(info_list[0].test_double);
    temp->set_test_string(info_list[0].test_string);
    temp->set_test_int32(info_list[0].test_int32);
    temp->set_test_sint32(info_list[0].test_sint32);
    temp->set_test_uint32(info_list[0].test_uint32);
    temp->set_test_int64(info_list[0].test_int64);
    temp->set_test_sint64(info_list[0].test_sint64);
    temp->set_test_uint64(info_list[0].test_uint64);

    temp = test_info_list.add_common_info_list();
    ASSERT_TRUE(temp != nullptr);
    temp->set_test_bool(info_list[1].test_bool);
    temp->set_test_float(info_list[1].test_float);
    temp->set_test_double(info_list[1].test_double);
    temp->set_test_string(info_list[1].test_string);
    temp->set_test_int32(info_list[1].test_int32);
    temp->set_test_sint32(info_list[1].test_sint32);
    temp->set_test_uint32(info_list[1].test_uint32);
    temp->set_test_int64(info_list[1].test_int64);
    temp->set_test_sint64(info_list[1].test_sint64);
    temp->set_test_uint64(info_list[1].test_uint64);

    // add_common_type_list
    for (auto val : type_list)
        test_info_list.add_common_type_list(val);

    ASSERT_TRUE(test_info_list.common_info_list_size() == static_cast<int>(info_list.size()));
    ASSERT_TRUE(test_info_list.common_type_list_size() == static_cast<int>(type_list.size()));

    // ByteSizeLong
    size_t test_info_list_size = test_info_list.ByteSizeLong();

    // SerializeToString & ParseFromString
    {
        std::string info_list_to_string;
        ASSERT_TRUE(test_info_list.SerializeToString(&info_list_to_string));
        ASSERT_TRUE(test_info_list_size == info_list_to_string.size());

        PBMsg::TestCommonInfoList string_to_info_list;
        ASSERT_TRUE(string_to_info_list.ParseFromString(info_list_to_string));
        ASSERT_TRUE(string_to_info_list.common_info_list_size() == static_cast<int>(info_list.size()));
        ASSERT_TRUE(string_to_info_list.common_type_list_size() == static_cast<int>(type_list.size()));
        ASSERT_TRUE(string_to_info_list.ByteSizeLong() == test_info_list_size);

        // foreach
        int idx = 0;
        for (const auto& val : string_to_info_list.common_info_list())
            ASSERT_TRUE(val == info_list[idx++]);

        // foreach
        idx = 0;
        for (const auto& val : string_to_info_list.common_type_list())
            ASSERT_TRUE(val == type_list[idx++]);

        // for
        for (idx = 0; idx != string_to_info_list.common_info_list_size(); ++idx)
            ASSERT_TRUE(string_to_info_list.common_info_list(idx) == info_list[idx]);

        // for
        for (idx = 0; idx != string_to_info_list.common_type_list_size(); ++idx)
            ASSERT_TRUE(string_to_info_list.common_type_list(idx) == type_list[idx]);

        // foreach mutable_common_info_list
        for (auto& val : *string_to_info_list.mutable_common_info_list())
            val.set_test_string("Mutable");

        // foreach mutable_common_type_list
        for (auto& val : *string_to_info_list.mutable_common_type_list())
            val = PBMsg::CT_FLOAT;

        // for mutable_common_info_list
        for (idx = 0; idx != string_to_info_list.common_info_list_size(); ++idx)
        {
            temp = string_to_info_list.mutable_common_info_list(idx);
            ASSERT_TRUE(temp != nullptr);
            temp->set_test_string(temp->test_string() + "CommonInfoString");
        }

        // for mutable_common_type_list
        if (auto ptr = string_to_info_list.mutable_common_type_list(); ptr != nullptr)
            for (idx = 0; idx != ptr->size(); ++idx)
                if (idx % 2 == 0)
                    (*ptr)[idx] = PBMsg::CT_DOUBLE;
    }

    // SerializeToArray & ParseFromArray
    {
        char info_list_to_array[128] = {0};
        ASSERT_TRUE(test_info_list.SerializeToArray(info_list_to_array, sizeof(info_list_to_array)));

        PBMsg::TestCommonInfoList array_to_info_list;
        ASSERT_TRUE(array_to_info_list.ParseFromArray(info_list_to_array, static_cast<int>(test_info_list_size)));
        ASSERT_TRUE(array_to_info_list.common_info_list_size() == static_cast<int>(info_list.size()));
        ASSERT_TRUE(array_to_info_list.common_type_list_size() == static_cast<int>(type_list.size()));
        ASSERT_TRUE(array_to_info_list.ByteSizeLong() == test_info_list_size);

        // foreach
        int idx = 0;
        for (const auto& val : array_to_info_list.common_info_list())
            ASSERT_TRUE(val == info_list[idx++]);

        // foreach
        idx = 0;
        for (const auto& val : array_to_info_list.common_type_list())
            ASSERT_TRUE(val == type_list[idx++]);

        // for
        for (idx = 0; idx != array_to_info_list.common_info_list_size(); ++idx)
            ASSERT_TRUE(array_to_info_list.common_info_list(idx) == info_list[idx]);

        // for
        for (idx = 0; idx != array_to_info_list.common_type_list_size(); ++idx)
            ASSERT_TRUE(array_to_info_list.common_type_list(idx) == type_list[idx]);

        // foreach mutable_common_info_list
        for (auto& val : *array_to_info_list.mutable_common_info_list())
            val.set_test_string("Mutable");

        // foreach mutable_common_type_list
        for (auto& val : *array_to_info_list.mutable_common_type_list())
            val = PBMsg::CT_DOUBLE;

        // for mutable_common_info_list
        for (idx = 0; idx != array_to_info_list.common_info_list_size(); ++idx)
        {
            temp = array_to_info_list.mutable_common_info_list(idx);
            ASSERT_TRUE(temp != nullptr);
            temp->set_test_string(temp->test_string() + "CommonInfoArray");
        }

        // for mutable_common_type_list
        if (auto ptr = array_to_info_list.mutable_common_type_list(); ptr != nullptr)
            for (idx = 0; idx != ptr->size(); ++idx)
                if (idx % 2 == 0)
                    (*ptr)[idx] = PBMsg::CT_FLOAT;
    }

    PBMsg::TestCommonInfoList test_info_list_copy = test_info_list;
    for (int idx = 0; idx != test_info_list_copy.common_info_list_size(); ++idx)
        ASSERT_TRUE(test_info_list_copy.common_info_list(idx) == info_list[idx]);

    for (int idx = 0; idx != test_info_list_copy.common_type_list_size(); ++idx)
        ASSERT_TRUE(test_info_list_copy.common_type_list(idx) == type_list[idx]);

    PBMsg::TestCommonInfoList test_info_list_move = std::move(test_info_list);
    for (int idx = 0; idx != test_info_list_move.common_info_list_size(); ++idx)
        ASSERT_TRUE(test_info_list_move.common_info_list(idx) == info_list[idx]);

    for (int idx = 0; idx != test_info_list_move.common_type_list_size(); ++idx)
        ASSERT_TRUE(test_info_list_move.common_type_list(idx) == type_list[idx]);

    ASSERT_TRUE(test_info_list.common_info_list_size() == 0);
    ASSERT_TRUE(test_info_list.common_type_list_size() == 0);
}