#include "test_protocol.h"

#include "fmt/format.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4127)
#endif

#include "TestMsgDefine.pb.h"
#include "TestMsgStruct.pb.h"

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

class CommonInfo
{
public:
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

bool operator ==(const CommonInfo& cinfo, const TestMsg::TestCommonInfo& pinfo)
{
    return cinfo.test_float  == pinfo.test_float()
        && cinfo.test_double == pinfo.test_double()
        && cinfo.test_string == pinfo.test_string()
        && cinfo.test_int32  == pinfo.test_int32()
        && cinfo.test_sint32 == pinfo.test_sint32()
        && cinfo.test_uint32 == pinfo.test_uint32()
        && cinfo.test_int64 == pinfo.test_int64()
        && cinfo.test_sint64 == pinfo.test_sint64()
        && cinfo.test_uint64 == pinfo.test_uint64();
}

bool operator ==(const TestMsg::TestCommonInfo& pinfo, const CommonInfo& cinfo)
{
    return cinfo == pinfo;
}

void test_protocol_enum(void)
{
    // CommonType_Name
    const std::string& str_float    = TestMsg::TestCommonType_Name(TestMsg::CT_FLOAT);
    const std::string& str_double   = TestMsg::TestCommonType_Name(TestMsg::CT_DOUBLE);
    const std::string& str_bytes    = TestMsg::TestCommonType_Name(TestMsg::CT_BYTES);
    const std::string& str_int32    = TestMsg::TestCommonType_Name(TestMsg::CT_INT32);
    const std::string& str_sint32   = TestMsg::TestCommonType_Name(TestMsg::CT_SINT32);
    const std::string& str_uint32   = TestMsg::TestCommonType_Name(TestMsg::CT_UINT32);
    const std::string& str_int64    = TestMsg::TestCommonType_Name(TestMsg::CT_INT64);
    const std::string& str_sint64   = TestMsg::TestCommonType_Name(TestMsg::CT_SINT64);
    const std::string& str_uint64   = TestMsg::TestCommonType_Name(TestMsg::CT_UINT64);
    const std::string& str_default1 = TestMsg::TestCommonType_Name(TestMsg::TestCommonType{});
    const std::string& str_default2 = TestMsg::TestCommonType_Name(TestMsg::TestCommonType(0));
    const std::string& str_null1    = TestMsg::TestCommonType_Name(TestMsg::TestCommonType(100));
    const std::string& str_null2    = TestMsg::TestCommonType_Name(TestMsg::TestCommonType(-1));

    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_FLOAT));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_DOUBLE));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_BYTES));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_INT32));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_SINT32));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_UINT32));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_INT64));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_SINT64));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::CT_UINT64));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::TestCommonType{}));
    abort_assert(TestMsg::TestCommonType_IsValid(TestMsg::TestCommonType(0)));
    abort_assert(!TestMsg::TestCommonType_IsValid(TestMsg::TestCommonType(100)));
    abort_assert(!TestMsg::TestCommonType_IsValid(TestMsg::TestCommonType(-1)));

    abort_assert(str_float    == "CT_FLOAT");
    abort_assert(str_double   == "CT_DOUBLE");
    abort_assert(str_bytes    == "CT_BYTES");
    abort_assert(str_int32    == "CT_INT32");
    abort_assert(str_sint32   == "CT_SINT32");
    abort_assert(str_uint32   == "CT_UINT32");
    abort_assert(str_int64    == "CT_INT64");
    abort_assert(str_sint64   == "CT_SINT64");
    abort_assert(str_uint64   == "CT_UINT64");
    abort_assert(str_default1 == "CT_FLOAT");
    abort_assert(str_default2 == "CT_FLOAT");
    abort_assert(str_null1.empty());
    abort_assert(str_null2.empty());

    // CommonType_Parse
    TestMsg::TestCommonType val{};
    abort_assert(TestMsg::CT_FLOAT == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_float, &val));
    abort_assert(TestMsg::CT_FLOAT == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_double, &val));
    abort_assert(TestMsg::CT_DOUBLE == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_bytes, &val));
    abort_assert(TestMsg::CT_BYTES == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_int32, &val));
    abort_assert(TestMsg::CT_INT32 == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_sint32, &val));
    abort_assert(TestMsg::CT_SINT32 == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_uint32, &val));
    abort_assert(TestMsg::CT_UINT32 == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_int64, &val));
    abort_assert(TestMsg::CT_INT64 == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_sint64, &val));
    abort_assert(TestMsg::CT_SINT64 == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_uint64, &val));
    abort_assert(TestMsg::CT_UINT64 == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_default1, &val));
    abort_assert(TestMsg::CT_FLOAT == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(TestMsg::TestCommonType_Parse(str_default2, &val));
    abort_assert(TestMsg::CT_FLOAT == val && TestMsg::TestCommonType_IsValid(val));

    abort_assert(!TestMsg::TestCommonType_Parse(str_null1, &val));
    abort_assert(!TestMsg::TestCommonType_Parse(str_null2, &val));

    fmt::print("{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __FUNCTION__)));
}

void test_protocol_serialization(void)
{
    CommonInfo info{1.23f, 4.5678, "CommonInfo", 123456, -123456, 123456, 567890, -567890, 567890};

    TestMsg::TestCommonInfo test_info;
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

    // SerializeToString & ParseFromString
    {
        std::string info_to_string;
        abort_assert(test_info.SerializeToString(&info_to_string));
        abort_assert(test_info_size == info_to_string.size());

        TestMsg::TestCommonInfo string_to_info;
        abort_assert(string_to_info.ParseFromString(info_to_string));
        abort_assert(string_to_info == info);
        abort_assert(string_to_info.ByteSizeLong() == test_info_size);
    }

    // SerializeToArray & ParseFromArray
    {
        char info_to_array[128] = {0};
        abort_assert(test_info.SerializeToArray(info_to_array, sizeof(info_to_array)));

        TestMsg::TestCommonInfo array_to_info;
        abort_assert(array_to_info.ParseFromArray(info_to_array, static_cast<int>(test_info_size)));
        abort_assert(array_to_info == info);
        abort_assert(array_to_info.ByteSizeLong() == test_info_size);
    }

    TestMsg::TestCommonInfo test_info_copy = test_info;
    abort_assert(test_info_copy == info);

    TestMsg::TestCommonInfo test_info_move = std::move(test_info);
    abort_assert(test_info_move == info);
    abort_assert(!(test_info == info));

    fmt::print("{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __FUNCTION__)));
}

void test_protocol_serialization_list(void)
{
    std::vector<CommonInfo> info_list{
        {1.23f, 4.5678, "CommonInfo1", 123456, -123456, 123456, 567890, -567890, 567890},
        {4.56f, 9.0123, "CommonInfo2", 456789, -456789, 456789, 123456, -123456, 123456},
    };

    std::vector<TestMsg::TestCommonType> type_list{
        TestMsg::CT_FLOAT,  TestMsg::CT_DOUBLE, TestMsg::CT_BYTES,
        TestMsg::CT_INT32,  TestMsg::CT_SINT32, TestMsg::CT_UINT32,
        TestMsg::CT_INT64,  TestMsg::CT_SINT64, TestMsg::CT_UINT64
    };

    // common_info_list_size
    TestMsg::TestCommonInfoList test_info_list;
    abort_assert(test_info_list.common_info_list_size() == 0);
    abort_assert(test_info_list.common_type_list_size() == 0);

    // add_common_info_list
    TestMsg::TestCommonInfo* temp = test_info_list.add_common_info_list();
    abort_assert(temp != nullptr);
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
    abort_assert(temp != nullptr);
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

    abort_assert(test_info_list.common_info_list_size() == static_cast<int>(info_list.size()));
    abort_assert(test_info_list.common_type_list_size() == static_cast<int>(type_list.size()));

    // ByteSizeLong
    size_t test_info_list_size = test_info_list.ByteSizeLong();
    fmt::print("test_info_list.ByteSizeLong={}\n", test_info_list_size);

    // SerializeToString & ParseFromString
    {
        std::string info_list_to_string;
        abort_assert(test_info_list.SerializeToString(&info_list_to_string));
        abort_assert(test_info_list_size == info_list_to_string.size());

        TestMsg::TestCommonInfoList string_to_info_list;
        abort_assert(string_to_info_list.ParseFromString(info_list_to_string));
        abort_assert(string_to_info_list.common_info_list_size() == static_cast<int>(info_list.size()));
        abort_assert(string_to_info_list.common_type_list_size() == static_cast<int>(type_list.size()));
        abort_assert(string_to_info_list.ByteSizeLong() == test_info_list_size);

        // foreach
        int idx = 0;
        for (const auto& val : string_to_info_list.common_info_list())
            abort_assert(val == info_list[idx++]);

        // foreach
        idx = 0;
        for (const auto& val : string_to_info_list.common_type_list())
            abort_assert(val == type_list[idx++]);

        // for
        for (idx = 0; idx != string_to_info_list.common_info_list_size(); ++idx)
            abort_assert(string_to_info_list.common_info_list(idx) == info_list[idx]);

        // for
        for (idx = 0; idx != string_to_info_list.common_type_list_size(); ++idx)
            abort_assert(string_to_info_list.common_type_list(idx) == type_list[idx]);

        // foreach mutable_common_info_list
        for (auto& val : *string_to_info_list.mutable_common_info_list())
            val.set_test_string("Mutable");

        // foreach mutable_common_type_list
        for (auto& val : *string_to_info_list.mutable_common_type_list())
            val = TestMsg::CT_FLOAT;

        // for mutable_common_info_list
        for (idx = 0; idx != string_to_info_list.common_info_list_size(); ++idx)
        {
            temp = string_to_info_list.mutable_common_info_list(idx);
            abort_assert(temp != nullptr);
            temp->set_test_string(temp->test_string() + "CommonInfoString");
        }

        // for mutable_common_type_list
        if (auto ptr = string_to_info_list.mutable_common_type_list(); ptr != nullptr)
            for (idx = 0; idx != ptr->size(); ++idx)
                if (idx % 2 == 0)
                    (*ptr)[idx] = TestMsg::CT_DOUBLE;
    }

    // SerializeToArray & ParseFromArray
    {
        char info_list_to_array[128] = {0};
        abort_assert(test_info_list.SerializeToArray(info_list_to_array, sizeof(info_list_to_array)));

        TestMsg::TestCommonInfoList array_to_info_list;
        abort_assert(array_to_info_list.ParseFromArray(info_list_to_array, static_cast<int>(test_info_list_size)));
        abort_assert(array_to_info_list.common_info_list_size() == static_cast<int>(info_list.size()));
        abort_assert(array_to_info_list.common_type_list_size() == static_cast<int>(type_list.size()));
        abort_assert(array_to_info_list.ByteSizeLong() == test_info_list_size);

        // foreach
        int idx = 0;
        for (const auto& val : array_to_info_list.common_info_list())
            abort_assert(val == info_list[idx++]);

        // foreach
        idx = 0;
        for (const auto& val : array_to_info_list.common_type_list())
            abort_assert(val == type_list[idx++]);

        // for
        for (idx = 0; idx != array_to_info_list.common_info_list_size(); ++idx)
            abort_assert(array_to_info_list.common_info_list(idx) == info_list[idx]);

        // for
        for (idx = 0; idx != array_to_info_list.common_type_list_size(); ++idx)
            abort_assert(array_to_info_list.common_type_list(idx) == type_list[idx]);

        // foreach mutable_common_info_list
        for (auto& val : *array_to_info_list.mutable_common_info_list())
            val.set_test_string("Mutable");

        // foreach mutable_common_type_list
        for (auto& val : *array_to_info_list.mutable_common_type_list())
            val = TestMsg::CT_DOUBLE;

        // for mutable_common_info_list
        for (idx = 0; idx != array_to_info_list.common_info_list_size(); ++idx)
        {
            temp = array_to_info_list.mutable_common_info_list(idx);
            abort_assert(temp != nullptr);
            temp->set_test_string(temp->test_string() + "CommonInfoArray");
        }

        // for mutable_common_type_list
        if (auto ptr = array_to_info_list.mutable_common_type_list(); ptr != nullptr)
            for (idx = 0; idx != ptr->size(); ++idx)
                if (idx % 2 == 0)
                    (*ptr)[idx] = TestMsg::CT_FLOAT;
    }

    TestMsg::TestCommonInfoList test_info_list_copy = test_info_list;
    for (int idx = 0; idx != test_info_list_copy.common_info_list_size(); ++idx)
        abort_assert(test_info_list_copy.common_info_list(idx) == info_list[idx]);

    for (int idx = 0; idx != test_info_list_copy.common_type_list_size(); ++idx)
        abort_assert(test_info_list_copy.common_type_list(idx) == type_list[idx]);

    TestMsg::TestCommonInfoList test_info_list_move = std::move(test_info_list);
    for (int idx = 0; idx != test_info_list_move.common_info_list_size(); ++idx)
        abort_assert(test_info_list_move.common_info_list(idx) == info_list[idx]);

    for (int idx = 0; idx != test_info_list_move.common_type_list_size(); ++idx)
        abort_assert(test_info_list_move.common_type_list(idx) == type_list[idx]);

    abort_assert(test_info_list.common_info_list_size() == 0);
    abort_assert(test_info_list.common_type_list_size() == 0);

    fmt::print("{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __FUNCTION__)));
}

void test_protocol_performance(void)
{
    CommonInfo info{1.23f, 4.5678, "CommonInfo", 123456, -123456, 123456, 567890, -567890, 567890};

    TestMsg::TestCommonInfo test_info;
    test_info.set_test_float(info.test_float);
    test_info.set_test_double(info.test_double);
    test_info.set_test_string(info.test_string);
    test_info.set_test_int32(info.test_int32);
    test_info.set_test_sint32(info.test_sint32);
    test_info.set_test_uint32(info.test_uint32);
    test_info.set_test_int64(info.test_int64);
    test_info.set_test_sint64(info.test_sint64);
    test_info.set_test_uint64(info.test_uint64);

    size_t test_info_size = test_info.ByteSizeLong();
    size_t repeat_times   = 100000;
    time_t start_time     = 0;
    time_t end_time       = 0;

    std::string info_to_string;
    char        info_to_array[128];

    // SerializeToString VS SerializeToArray
    {
        start_time = steady_clock_now();
        for (size_t idx = 0; idx != repeat_times; ++idx)
        {
            info_to_string.clear();
            abort_assert(test_info.SerializeToString(&info_to_string));
        }
        end_time = steady_clock_now();
        fmt::print("Repeat {} times, SerializeToString cost {} microsecond!!!\n", repeat_times, end_time - start_time);

        start_time = steady_clock_now();
        for (size_t idx = 0; idx != repeat_times; ++idx)
        {
            abort_assert(test_info.SerializeToArray(info_to_array, sizeof(info_to_array)));
        }
        end_time = steady_clock_now();
        fmt::print("Repeat {} times, SerializeToArray cost {} microsecond!!!\n", repeat_times, end_time - start_time);
    }

    // ParseFromString VS ParseFromArray
    {
        TestMsg::TestCommonInfo string_to_info;
        start_time = steady_clock_now();
        for (size_t idx = 0; idx != repeat_times; ++idx)
        {
            abort_assert(string_to_info.ParseFromString(info_to_string));
            abort_assert(string_to_info == info);
        }
        end_time = steady_clock_now();
        fmt::print("Repeat {} times, ParseFromString cost {} microsecond!!!\n", repeat_times, end_time - start_time);

        TestMsg::TestCommonInfo array_to_info;
        start_time = steady_clock_now();
        for (size_t idx = 0; idx != repeat_times; ++idx)
        {
            abort_assert(array_to_info.ParseFromArray(info_to_array, static_cast<int>(test_info_size)));
            abort_assert(array_to_info == info);
        }
        end_time = steady_clock_now();
        fmt::print("Repeat {} times, ParseFromArray cost {} microsecond!!!\n", repeat_times, end_time - start_time);
    }

    fmt::print("{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __FUNCTION__)));
}

void test_protocol_all(void)
{
    test_protocol_enum();
    test_protocol_serialization();
    test_protocol_serialization_list();
    test_protocol_performance();
}