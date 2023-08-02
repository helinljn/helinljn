#include "gtest/gtest.h"

#include "TestMsgDefine.pb.h"
#include "TestMsgStruct.pb.h"

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

bool operator ==(const CommonInfo& cinfo, const XXMsg::TestCommonInfo& pinfo)
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

bool operator ==(const XXMsg::TestCommonInfo& pinfo, const CommonInfo& cinfo)
{
    return cinfo == pinfo;
}

} // unnamed namespace

GTEST_TEST(ProtobufTest, EnumName)
{
    // CommonType_Name
    const std::string& str_bool     = XXMsg::TestCommonType_Name(XXMsg::CT_BOOL);
    const std::string& str_float    = XXMsg::TestCommonType_Name(XXMsg::CT_FLOAT);
    const std::string& str_double   = XXMsg::TestCommonType_Name(XXMsg::CT_DOUBLE);
    const std::string& str_bytes    = XXMsg::TestCommonType_Name(XXMsg::CT_BYTES);
    const std::string& str_int32    = XXMsg::TestCommonType_Name(XXMsg::CT_INT32);
    const std::string& str_sint32   = XXMsg::TestCommonType_Name(XXMsg::CT_SINT32);
    const std::string& str_uint32   = XXMsg::TestCommonType_Name(XXMsg::CT_UINT32);
    const std::string& str_int64    = XXMsg::TestCommonType_Name(XXMsg::CT_INT64);
    const std::string& str_sint64   = XXMsg::TestCommonType_Name(XXMsg::CT_SINT64);
    const std::string& str_uint64   = XXMsg::TestCommonType_Name(XXMsg::CT_UINT64);
    const std::string& str_default1 = XXMsg::TestCommonType_Name(XXMsg::TestCommonType{});
    const std::string& str_default2 = XXMsg::TestCommonType_Name(XXMsg::TestCommonType(0));
    const std::string& str_null1    = XXMsg::TestCommonType_Name(XXMsg::TestCommonType(100));
    const std::string& str_null2    = XXMsg::TestCommonType_Name(XXMsg::TestCommonType(-1));

    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_BOOL));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_FLOAT));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_DOUBLE));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_BYTES));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_INT32));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_SINT32));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_UINT32));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_INT64));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_SINT64));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::CT_UINT64));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::TestCommonType{}));
    ASSERT_TRUE(XXMsg::TestCommonType_IsValid(XXMsg::TestCommonType(0)));

    ASSERT_FALSE(XXMsg::TestCommonType_IsValid(XXMsg::TestCommonType(100)));
    ASSERT_FALSE(XXMsg::TestCommonType_IsValid(XXMsg::TestCommonType(-1)));

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
    XXMsg::TestCommonType val{};
    ASSERT_TRUE(XXMsg::CT_BOOL == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_bool, &val));
    ASSERT_TRUE(XXMsg::CT_BOOL == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_float, &val));
    ASSERT_TRUE(XXMsg::CT_FLOAT == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_double, &val));
    ASSERT_TRUE(XXMsg::CT_DOUBLE == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_bytes, &val));
    ASSERT_TRUE(XXMsg::CT_BYTES == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_int32, &val));
    ASSERT_TRUE(XXMsg::CT_INT32 == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_sint32, &val));
    ASSERT_TRUE(XXMsg::CT_SINT32 == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_uint32, &val));
    ASSERT_TRUE(XXMsg::CT_UINT32 == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_int64, &val));
    ASSERT_TRUE(XXMsg::CT_INT64 == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_sint64, &val));
    ASSERT_TRUE(XXMsg::CT_SINT64 == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_uint64, &val));
    ASSERT_TRUE(XXMsg::CT_UINT64 == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_default1, &val));
    ASSERT_TRUE(XXMsg::CT_BOOL == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_TRUE(XXMsg::TestCommonType_Parse(str_default2, &val));
    ASSERT_TRUE(XXMsg::CT_BOOL == val && XXMsg::TestCommonType_IsValid(val));

    ASSERT_FALSE(XXMsg::TestCommonType_Parse(str_null1, &val));
    ASSERT_FALSE(XXMsg::TestCommonType_Parse(str_null2, &val));
}

GTEST_TEST(ProtobufTest, Serialization)
{
    CommonInfo info{true, 1.23f, 4.5678, "CommonInfo", 123456, -123456, 123456, 567890, -567890, 567890};

    XXMsg::TestCommonInfo test_info;
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

        XXMsg::TestCommonInfo string_to_info;
        ASSERT_TRUE(string_to_info.ParseFromString(info_to_string));
        ASSERT_TRUE(string_to_info == info);
        ASSERT_TRUE(string_to_info.ByteSizeLong() == test_info_size);
    }

    // SerializeToArray & ParseFromArray
    {
        char info_to_array[128] = {0};
        ASSERT_TRUE(test_info.SerializeToArray(info_to_array, sizeof(info_to_array)));

        XXMsg::TestCommonInfo array_to_info;
        ASSERT_TRUE(array_to_info.ParseFromArray(info_to_array, static_cast<int>(test_info_size)));
        ASSERT_TRUE(array_to_info == info);
        ASSERT_TRUE(array_to_info.ByteSizeLong() == test_info_size);
    }

    XXMsg::TestCommonInfo test_info_copy = test_info;
    ASSERT_TRUE(test_info_copy == info);

    XXMsg::TestCommonInfo test_info_move = std::move(test_info);
    ASSERT_TRUE(test_info_move == info);

    ASSERT_FALSE(test_info == info);
}

GTEST_TEST(ProtobufTest, SerializationList)
{
    std::vector<CommonInfo> info_list{
        {true, 1.23f, 4.5678, "CommonInfo1", 123456, -123456, 123456, 567890, -567890, 567890},
        {true, 4.56f, 9.0123, "CommonInfo2", 456789, -456789, 456789, 123456, -123456, 123456},
    };

    std::vector<XXMsg::TestCommonType> type_list{
        XXMsg::CT_BOOL,   XXMsg::CT_FLOAT,  XXMsg::CT_DOUBLE, XXMsg::CT_BYTES,  XXMsg::CT_INT32,
        XXMsg::CT_SINT32, XXMsg::CT_UINT32, XXMsg::CT_INT64,  XXMsg::CT_SINT64, XXMsg::CT_UINT64
    };

    // common_info_list_size
    XXMsg::TestCommonInfoList test_info_list;
    ASSERT_TRUE(test_info_list.common_info_list_size() == 0);
    ASSERT_TRUE(test_info_list.common_type_list_size() == 0);

    // add_common_info_list
    XXMsg::TestCommonInfo* temp = test_info_list.add_common_info_list();
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

        XXMsg::TestCommonInfoList string_to_info_list;
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
            val = XXMsg::CT_FLOAT;

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
                    (*ptr)[idx] = XXMsg::CT_DOUBLE;
    }

    // SerializeToArray & ParseFromArray
    {
        char info_list_to_array[128] = {0};
        ASSERT_TRUE(test_info_list.SerializeToArray(info_list_to_array, sizeof(info_list_to_array)));

        XXMsg::TestCommonInfoList array_to_info_list;
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
            val = XXMsg::CT_DOUBLE;

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
                    (*ptr)[idx] = XXMsg::CT_FLOAT;
    }

    XXMsg::TestCommonInfoList test_info_list_copy = test_info_list;
    for (int idx = 0; idx != test_info_list_copy.common_info_list_size(); ++idx)
        ASSERT_TRUE(test_info_list_copy.common_info_list(idx) == info_list[idx]);

    for (int idx = 0; idx != test_info_list_copy.common_type_list_size(); ++idx)
        ASSERT_TRUE(test_info_list_copy.common_type_list(idx) == type_list[idx]);

    XXMsg::TestCommonInfoList test_info_list_move = std::move(test_info_list);
    for (int idx = 0; idx != test_info_list_move.common_info_list_size(); ++idx)
        ASSERT_TRUE(test_info_list_move.common_info_list(idx) == info_list[idx]);

    for (int idx = 0; idx != test_info_list_move.common_type_list_size(); ++idx)
        ASSERT_TRUE(test_info_list_move.common_type_list(idx) == type_list[idx]);

    ASSERT_TRUE(test_info_list.common_info_list_size() == 0);
    ASSERT_TRUE(test_info_list.common_type_list_size() == 0);
}