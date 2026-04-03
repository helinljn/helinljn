#include "doctest.h"
#include "json/json.h"
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

/********************************************************************
 * 测试用例 1：基本 Value 创建与类型判断
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Basic Value Creation and Type Checks")
{
    // 默认构造的 Value 为 null
    Json::Value nullValue;
    DOCTEST_CHECK(nullValue.isNull());
    DOCTEST_CHECK(nullValue.empty());

    // 布尔/整型/浮点/字符串/数组/对象
    Json::Value b(true);
    DOCTEST_CHECK(b.isBool());
    DOCTEST_CHECK(b.asBool() == true);

    Json::Value i(42);
    DOCTEST_CHECK(i.isInt());
    DOCTEST_CHECK(i.asInt() == 42);

    Json::Value d(3.14159);
    DOCTEST_CHECK(d.isDouble());
    DOCTEST_CHECK(d.asDouble() == doctest::Approx(3.14159).epsilon(1e-6));

    Json::Value s("hello");
    DOCTEST_CHECK(s.isString());
    DOCTEST_CHECK(s.asString() == "hello");

    Json::Value arr(Json::arrayValue);
    DOCTEST_CHECK(arr.isArray());
    DOCTEST_CHECK(arr.empty());

    Json::Value obj(Json::objectValue);
    DOCTEST_CHECK(obj.isObject());
    DOCTEST_CHECK(obj.empty());
}

/********************************************************************
 * 测试用例 2：数组操作（append, index, size, clear, 遍历）
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Array operations")
{
    Json::Value arr(Json::arrayValue);

    // append 不同类型
    arr.append(1);
    arr.append("two");
    arr.append(true);
    arr.append(4.5);

    DOCTEST_CHECK(arr.size() == 4);
    DOCTEST_CHECK(arr[0].asInt() == 1);
    DOCTEST_CHECK(arr[1].asString() == "two");
    DOCTEST_CHECK(arr[2].asBool() == true);
    DOCTEST_CHECK(arr[3].asDouble() == doctest::Approx(4.5).epsilon(1e-6));

    // 使用下标赋值可以扩展数组
    arr[4] = "five";
    DOCTEST_CHECK(arr.size() == 5);
    DOCTEST_CHECK(arr[4].asString() == "five");

    // 遍历数组（iterator/索引）
    int cnt = 0;
    for (Json::Value::iterator it = arr.begin(); it != arr.end(); ++it) {
        (void)it;
        cnt++;
    }
    DOCTEST_CHECK(cnt == 5);

    // clear 将移除所有元素
    arr.clear();
    DOCTEST_CHECK(arr.empty());
}

/********************************************************************
 * 测试用例 3：对象操作（成员增删、isMember、getMemberNames）
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Object operations")
{
    Json::Value o(Json::objectValue);

    // 赋值与类型推断
    o["name"] = "Alice";
    o["age"] = 30;
    o["score"] = 99.5;
    o["active"] = true;

    DOCTEST_CHECK(o.isMember("name"));
    DOCTEST_CHECK(o["name"].asString() == "Alice");
    DOCTEST_CHECK(o["age"].asInt() == 30);

    // getMemberNames 返回成员名向量（顺序未严格保证）
    Json::Value::Members members = o.getMemberNames();
    DOCTEST_CHECK(members.size() == 4);

    // 删除成员：removeMember
    o.removeMember("active");
    DOCTEST_CHECK(!o.isMember("active"));

    // 嵌套对象
    Json::Value addr(Json::objectValue);
    addr["city"] = "Beijing";
    addr["zip"] = "100000";
    o["address"] = addr;
    DOCTEST_CHECK(o["address"]["city"].asString() == "Beijing");
}

/********************************************************************
 * 测试用例 4：从字符串解析 JSON（正确与错误场景）
 * 使用 Json::CharReaderBuilder / parseFromStream（推荐的解析方式）
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Parsing from string (CharReaderBuilder)")
{
    const std::string jsonText = R"({
        "name": "Bob",
        "age": 25,
        "grades": [88, 92, 79],
        "meta": {"active": true}
    })";

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    std::istringstream iss(jsonText);
    bool ok = Json::parseFromStream(builder, iss, &root, &errs);
    DOCTEST_CHECK(ok);
    DOCTEST_CHECK(errs.empty());

    DOCTEST_CHECK(root["name"].asString() == "Bob");
    DOCTEST_CHECK(root["age"].asInt() == 25);
    DOCTEST_CHECK(root["grades"].isArray());
    DOCTEST_CHECK(root["grades"][1].asInt() == 92);

    // 解析错误示例：截断的 JSON
    const std::string bad = R"({"a": 1,)";
    Json::Value badRoot;
    std::istringstream iss2(bad);
    std::string errs2;
    bool ok2 = Json::parseFromStream(builder, iss2, &badRoot, &errs2);
    DOCTEST_CHECK(!ok2);
    DOCTEST_CHECK(!errs2.empty());
}

/********************************************************************
 * 测试用例 5：序列化写出（FastWriter、StyledWriter、StreamWriterBuilder）
 * 覆盖旧/新写法，展示常用配置
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Writing JSON (FastWriter / StyledWriter / StreamWriterBuilder)")
{
    Json::Value root(Json::objectValue);
    root["user"] = "test";
    root["val"] = 123;

    // FastWriter（非格式化，历史接口）
    Json::FastWriter fast;
    std::string fastOut = fast.write(root);
    DOCTEST_CHECK(!fastOut.empty());

    // StyledWriter（可读性更好，历史接口）
    Json::StyledWriter styled;
    std::string styledOut = styled.write(root);
    DOCTEST_CHECK(!styledOut.empty());
    DOCTEST_CHECK(styledOut.find("\n") != std::string::npos); // 有换行和缩进

    // StreamWriterBuilder（现代接口，可配置）
    Json::StreamWriterBuilder swb;
    swb["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(swb.newStreamWriter());
    std::ostringstream oss;
    writer->write(root, &oss);
    std::string streamOut = oss.str();
    DOCTEST_CHECK(!streamOut.empty());
    DOCTEST_CHECK(streamOut.find("\"user\"") != std::string::npos);
}

/********************************************************************
 * 测试用例 6：类型转换行为与异常（asInt/asDouble/asString/asBool）
 * 说明 JsonCpp 在类型转换上的常见行为与兼容性注意事项
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Type conversion and exceptions")
{
    Json::Value v;

    // 从数字转换
    v = 100;
    DOCTEST_CHECK(v.asInt() == 100);
    DOCTEST_CHECK(v.asDouble() == 100.0);
    DOCTEST_CHECK(v.asString() == "100");

    // 字符串类型的 asInt/asDouble 在某些版本会抛出 Json::LogicError
    v = "123";
    DOCTEST_CHECK(v.isString());
    // 在多数 JsonCpp 版本中，对字符串调用 asInt() 会抛出 Json::LogicError
    DOCTEST_CHECK_THROWS_AS(v.asInt(), Json::LogicError);
    DOCTEST_CHECK_THROWS_AS(v.asDouble(), Json::LogicError);
    DOCTEST_CHECK(v.asString() == "123");

    // 布尔转换
    v = true;
    DOCTEST_CHECK(v.asBool() == true);
    DOCTEST_CHECK(v.asInt() == 1);
    DOCTEST_CHECK(v.asString() == "true");

    // 空值（null）的默认转换行为（通常返回 0 / false / ""）
    v = Json::Value();
    DOCTEST_CHECK(v.isNull());
    DOCTEST_CHECK(v.asInt() == 0);
    DOCTEST_CHECK(v.asUInt() == 0u);
    DOCTEST_CHECK(v.asBool() == false);
    DOCTEST_CHECK(v.asDouble() == 0.0);
    DOCTEST_CHECK(v.asString() == "");
}

/********************************************************************
 * 测试用例 7：解析特性开关（注释、尾随逗号、单引号）
 * 演示如何通过 CharReaderBuilder 配置解析规则
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Parsing features: comments, trailing commas, single quotes")
{
    // 含注释的 JSON（C++ 风格注释和多行注释）
    const std::string withComments = R"(
        {
            // line comment
            "a": 1 /* block comment */
        }
    )";
    Json::CharReaderBuilder b1;
    b1["allowComments"] = true;
    std::string e1;
    Json::Value r1;
    std::istringstream iss_withComments(withComments);
    DOCTEST_CHECK(Json::parseFromStream(b1, iss_withComments, &r1, &e1));
    DOCTEST_CHECK(r1["a"].asInt() == 1);

    // 尾随逗号
    const std::string withTrailing = R"({
        "x": 1,
    })";
    Json::CharReaderBuilder b2;
    b2["allowTrailingCommas"] = true;
    std::string e2;
    Json::Value r2;
    std::istringstream iss_withTrailing(withTrailing);
    DOCTEST_CHECK(Json::parseFromStream(b2, iss_withTrailing, &r2, &e2));
    DOCTEST_CHECK(r2["x"].asInt() == 1);

    // 单引号
    const std::string withSingle = R"({'k':'v'})";
    Json::CharReaderBuilder b3;
    b3["allowSingleQuotes"] = true;
    std::string e3;
    Json::Value r3;
    std::istringstream iss_withSingle(withSingle);
    DOCTEST_CHECK(Json::parseFromStream(b3, iss_withSingle, &r3, &e3));
    DOCTEST_CHECK(r3["k"].asString() == "v");
}

/********************************************************************
 * 测试用例 8：错误处理与 get() 的默认值
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Error handling and default get")
{
    // 当 JSON 字段类型与期望不匹配时，解析仍然可能成功（但转换抛异常）
    const std::string json = R"({"age": "unknown"})";
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss_json(json);
    DOCTEST_CHECK(Json::parseFromStream(builder, iss_json, &root, &errs));
    DOCTEST_CHECK(root["age"].isString());
    DOCTEST_CHECK_THROWS_AS(root["age"].asInt(), Json::LogicError);

    // get(key, default) 在不存在成员时返回默认值（默认值以 Json::Value 形式提供）
    const Json::Value& dv = root.get("no", Json::Value("def"));
    DOCTEST_CHECK(dv.asString() == "def");
}

/********************************************************************
 * 测试用例 9：复杂嵌套结构的序列化/反序列化
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Complex nested structures and roundtrip")
{
    Json::Value root(Json::objectValue);
    Json::Value items(Json::arrayValue);

    for (int i = 0; i < 3; ++i) {
        Json::Value it(Json::objectValue);
        it["id"] = i;
        it["name"] = "n" + std::to_string(i);
        items.append(it);
    }

    root["items"] = items;
    root["meta"]["version"] = "1.0";

    // 序列化并反序列化
    Json::StreamWriterBuilder swb;
    std::ostringstream oss;
    std::unique_ptr<Json::StreamWriter> w(swb.newStreamWriter());
    w->write(root, &oss);
    std::string out = oss.str();
    DOCTEST_CHECK(!out.empty());

    Json::Value parsed;
    Json::CharReaderBuilder rb;
    std::string errs;
    std::istringstream iss_out(out);
    DOCTEST_CHECK(Json::parseFromStream(rb, iss_out, &parsed, &errs));
    DOCTEST_CHECK(parsed["items"].size() == 3);
    DOCTEST_CHECK(parsed["meta"]["version"].asString() == "1.0");
}

/********************************************************************
 * 测试用例 10：Json::Path 使用示例（路径解析、默认值）
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Json::Path usage")
{
    Json::Value r(Json::objectValue);
    r["person"]["name"] = "Ann";
    r["person"]["age"] = 40;
    r["arr"] = Json::Value(Json::arrayValue);
    r["arr"].append(10);
    r["arr"].append(20);

    Json::Path pName(".person.name");
    Json::Path pAge(".person.age");
    Json::Path pIdx(".arr.[1]");

    DOCTEST_CHECK(pName.resolve(r).asString() == "Ann");
    DOCTEST_CHECK(pAge.resolve(r).asInt() == 40);
    DOCTEST_CHECK(pIdx.resolve(r).asInt() == 20);

    // 默认值示例：resolve(root, defaultValue)
    Json::Value def("missing");
    DOCTEST_CHECK(Json::Path(".person.missing").resolve(r, def).asString() == "missing");
}

/********************************************************************
 * 测试用例 11：大量数据构建与简单行为验证（性能/内存敏感场景）
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Large structures and roundtrip sanity")
{
    Json::Value big(Json::arrayValue);
    const int N = 1000; // 适中数量，避免测试超时
    for (int i = 0; i < N; ++i) {
        Json::Value it(Json::objectValue);
        it["index"] = i;
        it["text"] = "entry_" + std::to_string(i);
        big.append(it);
    }
    DOCTEST_CHECK(big.size() == (size_t)N);

    // 序列化/反序列化检查大小一致
    Json::FastWriter fw;
    std::string s = fw.write(big);
    DOCTEST_CHECK(!s.empty());

    Json::Value parsed;
    Json::CharReaderBuilder rb;
    std::string errs;
    std::istringstream iss_s(s);
    DOCTEST_CHECK(Json::parseFromStream(rb, iss_s, &parsed, &errs));
    DOCTEST_CHECK(parsed.size() == (size_t)N);
}