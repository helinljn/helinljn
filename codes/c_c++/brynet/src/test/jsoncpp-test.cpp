#include "doctest.h"
#include "json/json.h"
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <limits>

/********************************************************************
 * 测试用例 1：基本 Value 创建与类型判断
 * 覆盖所有 is*() 方法及 asUInt/asInt64/asUInt64/asFloat
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Basic Value Creation and Type Checks")
{
    // 默认构造的 Value 为 null
    Json::Value nullValue;
    DOCTEST_CHECK(nullValue.isNull());
    DOCTEST_CHECK(nullValue.empty());

    // 布尔
    Json::Value b(true);
    DOCTEST_CHECK(b.isBool());
    DOCTEST_CHECK(b.asBool() == true);

    // 整型（正数）
    Json::Value i(42);
    DOCTEST_CHECK(i.isInt());
    DOCTEST_CHECK(i.isUInt());      // 42 同时满足 isUInt
    DOCTEST_CHECK(i.isIntegral());  // 整数也是 integral
    DOCTEST_CHECK(i.isNumeric());   // 整数也是 numeric
    DOCTEST_CHECK(i.asInt() == 42);
    DOCTEST_CHECK(i.asUInt() == 42u);

    // 整型（负数，不满足 isUInt）
    Json::Value neg(-1);
    DOCTEST_CHECK(neg.isInt());
    DOCTEST_CHECK(!neg.isUInt());   // 负数不是 UInt
    DOCTEST_CHECK(neg.isIntegral());
    DOCTEST_CHECK(neg.asInt() == -1);

    // 无符号整数
    Json::Value u(static_cast<Json::UInt>(300));
    DOCTEST_CHECK(u.isUInt());
    DOCTEST_CHECK(u.isIntegral());
    DOCTEST_CHECK(u.asUInt() == 300u);

    // Int64 / UInt64
    Json::Value i64(static_cast<Json::Int64>(123456789012LL));
    DOCTEST_CHECK(i64.isInt64());
    DOCTEST_CHECK(i64.isIntegral());
    DOCTEST_CHECK(i64.asInt64() == 123456789012LL);

    Json::Value u64(static_cast<Json::UInt64>(123456789012ULL));
    DOCTEST_CHECK(u64.isUInt64());
    DOCTEST_CHECK(u64.isIntegral());
    DOCTEST_CHECK(u64.asUInt64() == 123456789012ULL);

    // 浮点
    Json::Value d(3.14159);
    DOCTEST_CHECK(d.isDouble());
    DOCTEST_CHECK(d.isNumeric());   // 浮点也是 numeric
    DOCTEST_CHECK(!d.isIntegral()); // 浮点不是 integral（除非恰好是整数值，视版本而定）
    DOCTEST_CHECK(d.asDouble() == doctest::Approx(3.14159).epsilon(1e-6));
    DOCTEST_CHECK(d.asFloat() == doctest::Approx(3.14159f).epsilon(1e-5f));

    // 字符串
    Json::Value s("hello");
    DOCTEST_CHECK(s.isString());
    DOCTEST_CHECK(s.asString() == "hello");

    // 数组
    Json::Value arr(Json::arrayValue);
    DOCTEST_CHECK(arr.isArray());
    DOCTEST_CHECK(arr.empty());

    // 对象
    Json::Value obj(Json::objectValue);
    DOCTEST_CHECK(obj.isObject());
    DOCTEST_CHECK(obj.empty());

    // isConvertibleTo 测试：检查类型之间的可转换性
    DOCTEST_CHECK(nullValue.isConvertibleTo(Json::nullValue));
    DOCTEST_CHECK(nullValue.isConvertibleTo(Json::intValue));     // null 可转为 int（结果为 0）
    DOCTEST_CHECK(nullValue.isConvertibleTo(Json::booleanValue)); // null 可转为 bool（结果为 false）
    DOCTEST_CHECK(!s.isConvertibleTo(Json::intValue));            // 字符串不可转为 int
}

/********************************************************************
 * 测试用例 2：数组操作（append, index, size, clear, 遍历, 越界访问）
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

    // 越界下标自动填充 null：arr[7] 跳过索引 5、6
    arr[7] = "eight";
    DOCTEST_CHECK(arr.size() == 8);
    DOCTEST_CHECK(arr[5].isNull());  // 自动填充的中间元素为 null
    DOCTEST_CHECK(arr[6].isNull());
    DOCTEST_CHECK(arr[7].asString() == "eight");

    // 遍历数组（iterator），验证解引用
    int cnt = 0;
    for (Json::Value::iterator it = arr.begin(); it != arr.end(); ++it)
    {
        int idx = static_cast<int>(it - arr.begin());
        if (idx != 5 && idx != 6)
            DOCTEST_CHECK(!it->isNull());
        cnt++;
    }
    DOCTEST_CHECK(cnt == 8);

    // const_iterator 遍历
    const Json::Value& carr = arr;
    int ccnt = 0;
    for (Json::Value::const_iterator cit = carr.begin(); cit != carr.end(); ++cit)
    {
        ccnt++;
    }
    DOCTEST_CHECK(ccnt == 8);

    // 索引遍历验证具体值
    DOCTEST_CHECK(arr[0].asInt() == 1);
    DOCTEST_CHECK(arr[1].asString() == "two");
    DOCTEST_CHECK(arr[2].asBool() == true);
    DOCTEST_CHECK(arr[4].asString() == "five");
    DOCTEST_CHECK(arr[7].asString() == "eight");

    // clear 将移除所有元素
    arr.clear();
    DOCTEST_CHECK(arr.empty());
}

/********************************************************************
 * 测试用例 3：对象操作（成员增删、isMember、getMemberNames、特殊key）
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
    DOCTEST_CHECK(o.size() == 3);

    // 嵌套对象
    Json::Value addr(Json::objectValue);
    addr["city"] = "Beijing";
    addr["zip"] = "100000";
    o["address"] = addr;
    DOCTEST_CHECK(o["address"]["city"].asString() == "Beijing");

    // 特殊字符 key（含中文、空格、转义字符）
    o["城市"] = "上海";
    o["key with space"] = "val";
    o["key\"quote"] = "qv";
    DOCTEST_CHECK(o["城市"].asString() == "上海");
    DOCTEST_CHECK(o["key with space"].asString() == "val");
    DOCTEST_CHECK(o["key\"quote"].asString() == "qv");

    // 空字符串 key
    o[""] = "empty_key";
    DOCTEST_CHECK(o.isMember(""));
    DOCTEST_CHECK(o[""].asString() == "empty_key");
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

    // 重复 key 解析：后值覆盖前值
    const std::string dupKey = R"({"x": 1, "x": 2})";
    Json::Value dupRoot;
    std::istringstream iss3(dupKey);
    std::string errs3;
    bool ok3 = Json::parseFromStream(builder, iss3, &dupRoot, &errs3);
    DOCTEST_CHECK(ok3);
    DOCTEST_CHECK(dupRoot["x"].asInt() == 2); // 后值覆盖
}

/********************************************************************
 * 测试用例 5：序列化写出（StreamWriterBuilder 为主，标注旧接口 deprecated）
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Writing JSON (StreamWriterBuilder and legacy writers)")
{
    Json::Value root(Json::objectValue);
    root["user"] = "test";
    root["val"] = 123;

    // FastWriter（已弃用，历史接口，仅验证兼容性）
    // 注意：jsoncpp 1.9.x 中 FastWriter/StyledWriter 已标记为 deprecated
    Json::FastWriter fast;
    std::string fastOut = fast.write(root);
    DOCTEST_CHECK(!fastOut.empty());
    DOCTEST_CHECK(fastOut.find("\"user\"") != std::string::npos);

    // StyledWriter（已弃用，历史接口）
    Json::StyledWriter styled;
    std::string styledOut = styled.write(root);
    DOCTEST_CHECK(!styledOut.empty());
    DOCTEST_CHECK(styledOut.find("\n") != std::string::npos);

    // StreamWriterBuilder（现代推荐接口，可配置）
    // 默认配置（无缩进，紧凑输出）
    {
        Json::StreamWriterBuilder swb;
        std::unique_ptr<Json::StreamWriter> writer(swb.newStreamWriter());
        std::ostringstream oss;
        writer->write(root, &oss);
        std::string streamOut = oss.str();
        DOCTEST_CHECK(!streamOut.empty());
        DOCTEST_CHECK(streamOut.find("\"user\"") != std::string::npos);
    }

    // 自定义缩进
    {
        Json::StreamWriterBuilder swb;
        swb["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(swb.newStreamWriter());
        std::ostringstream oss;
        writer->write(root, &oss);
        std::string streamOut = oss.str();
        DOCTEST_CHECK(streamOut.find("  ") != std::string::npos); // 应有缩进
        DOCTEST_CHECK(streamOut.find("\n") != std::string::npos); // 应有换行
    }

    // emitUTF8 配置
    {
        Json::StreamWriterBuilder swb;
        swb["emitUTF8"] = true;
        std::unique_ptr<Json::StreamWriter> writer(swb.newStreamWriter());
        std::ostringstream oss;
        Json::Value v("中文");
        writer->write(v, &oss);
        DOCTEST_CHECK(!oss.str().empty());
    }

    // enableYAMLCompatibility 配置
    {
        Json::StreamWriterBuilder swb;
        swb["enableYAMLCompatibility"] = true;
        swb["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(swb.newStreamWriter());
        std::ostringstream oss;
        writer->write(root, &oss);
        DOCTEST_CHECK(!oss.str().empty());
    }
}

/********************************************************************
 * 测试用例 6：类型转换行为（asInt/asDouble/asString/asBool）
 * jsoncpp 1.9.7 的 as*() 方法支持广泛隐式类型转换：
 *   - null  → asInt=0, asUInt=0, asDouble=0.0, asBool=false, asString=""
 *   - bool  → asInt=1/0, asDouble=1.0/0.0, asString="true"/"false"
 *   - int   → asUInt(范围内), asDouble, asBool(非零true), asString="数字"
 *   - double→ asInt(范围内), asBool(非零且非NaN), asString="数字"
 *   - string→ asInt/asDouble/asBool/asUInt 等抛 Json::LogicError
 *   - array/object → 除同类型外均抛 Json::LogicError
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Type conversion and exceptions")
{
    Json::Value v;

    // 整数类型：可隐式转换为多种类型
    v = 100;
    DOCTEST_CHECK(v.asInt() == 100);
    DOCTEST_CHECK(v.asUInt() == 100u);
    DOCTEST_CHECK(v.asDouble() == 100.0);
    DOCTEST_CHECK(v.asInt64() == 100);
    DOCTEST_CHECK(v.asUInt64() == 100u);
    DOCTEST_CHECK(v.asBool() == true);     // 非零整数转为 true
    DOCTEST_CHECK(v.asString() == "100");  // 整数转字符串

    // 整数 0 转为 bool 为 false
    v = 0;
    DOCTEST_CHECK(v.asBool() == false);

    // 字符串类型：只能安全调用 asString，其他 as*() 抛 LogicError
    v = "123";
    DOCTEST_CHECK(v.isString());
    DOCTEST_CHECK(v.asString() == "123");
    DOCTEST_CHECK_THROWS_AS(v.asInt(), Json::LogicError);
    DOCTEST_CHECK_THROWS_AS(v.asDouble(), Json::LogicError);
    DOCTEST_CHECK_THROWS_AS(v.asUInt(), Json::LogicError);
    DOCTEST_CHECK_THROWS_AS(v.asBool(), Json::LogicError);

    // 布尔类型：支持隐式转换
    v = true;
    DOCTEST_CHECK(v.asBool() == true);
    DOCTEST_CHECK(v.asInt() == 1);           // true → 1
    DOCTEST_CHECK(v.asDouble() == 1.0);      // true → 1.0
    DOCTEST_CHECK(v.asString() == "true");   // true → "true"

    v = false;
    DOCTEST_CHECK(v.asBool() == false);
    DOCTEST_CHECK(v.asInt() == 0);           // false → 0
    DOCTEST_CHECK(v.asString() == "false");  // false → "false"

    // 空值（null）：支持隐式转换，返回各类型的零值
    v = Json::Value();
    DOCTEST_CHECK(v.isNull());
    DOCTEST_CHECK(v.asInt() == 0);
    DOCTEST_CHECK(v.asUInt() == 0u);
    DOCTEST_CHECK(v.asDouble() == 0.0);
    DOCTEST_CHECK(v.asBool() == false);
    DOCTEST_CHECK(v.asString() == "");

    // 浮点类型：支持隐式转换
    v = 3.14;
    DOCTEST_CHECK(v.asDouble() == doctest::Approx(3.14));
    DOCTEST_CHECK(v.asBool() == true);
    DOCTEST_CHECK(v.asString().find("3.14") != std::string::npos); // 浮点转字符串

    // 浮点 0.0 转为 bool 为 false
    v = 0.0;
    DOCTEST_CHECK(v.asBool() == false);

    // 数组/对象类型：调用不兼容的 as*() 抛 LogicError
    v = Json::Value(Json::arrayValue);
    DOCTEST_CHECK_THROWS_AS(v.asInt(), Json::LogicError);
    DOCTEST_CHECK_THROWS_AS(v.asString(), Json::LogicError);

    v = Json::Value(Json::objectValue);
    DOCTEST_CHECK_THROWS_AS(v.asInt(), Json::LogicError);
    DOCTEST_CHECK_THROWS_AS(v.asString(), Json::LogicError);
}

/********************************************************************
 * 测试用例 7：解析特性开关（注释、尾随逗号、单引号）
 * 演示如何通过 CharReaderBuilder 配置解析规则，
 * 同时验证默认配置下这些特性是禁用的
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

    // 默认 CharReaderBuilder 配置下注释即可解析（allowComments 默认兼容旧 Reader 行为）
    // 显式关闭 allowComments 后应解析失败
    {
        Json::CharReaderBuilder b;
        b["allowComments"] = false;
        std::string e;
        Json::Value r;
        std::istringstream iss(withComments);
        DOCTEST_CHECK(!Json::parseFromStream(b, iss, &r, &e));
    }

    // 显式开启 allowComments 后应解析成功
    {
        Json::CharReaderBuilder b;
        b["allowComments"] = true;
        std::string e;
        Json::Value r;
        std::istringstream iss(withComments);
        DOCTEST_CHECK(Json::parseFromStream(b, iss, &r, &e));
        DOCTEST_CHECK(r["a"].asInt() == 1);
    }

    // 尾随逗号
    const std::string withTrailing = R"({
        "x": 1,
    })";

    // 显式关闭 allowTrailingCommas 后应解析失败
    {
        Json::CharReaderBuilder b;
        b["allowTrailingCommas"] = false;
        std::string e;
        Json::Value r;
        std::istringstream iss(withTrailing);
        DOCTEST_CHECK(!Json::parseFromStream(b, iss, &r, &e));
    }

    // 开启 allowTrailingCommas 后应解析成功
    {
        Json::CharReaderBuilder b;
        b["allowTrailingCommas"] = true;
        std::string e;
        Json::Value r;
        std::istringstream iss(withTrailing);
        DOCTEST_CHECK(Json::parseFromStream(b, iss, &r, &e));
        DOCTEST_CHECK(r["x"].asInt() == 1);
    }

    // 单引号
    const std::string withSingle = R"({'k':'v'})";

    // 显式关闭 allowSingleQuotes 后应解析失败
    {
        Json::CharReaderBuilder b;
        b["allowSingleQuotes"] = false;
        std::string e;
        Json::Value r;
        std::istringstream iss(withSingle);
        DOCTEST_CHECK(!Json::parseFromStream(b, iss, &r, &e));
    }

    // 开启 allowSingleQuotes 后应解析成功
    {
        Json::CharReaderBuilder b;
        b["allowSingleQuotes"] = true;
        std::string e;
        Json::Value r;
        std::istringstream iss(withSingle);
        DOCTEST_CHECK(Json::parseFromStream(b, iss, &r, &e));
        DOCTEST_CHECK(r["k"].asString() == "v");
    }
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

    // get(key, default) 在不存在成员时返回默认值
    const Json::Value& dv = root.get("no", Json::Value("def"));
    DOCTEST_CHECK(dv.asString() == "def");

    // get(key, default) 在成员存在时返回实际值（忽略默认值）
    const Json::Value& existing = root.get("age", Json::Value(0));
    DOCTEST_CHECK(existing.asString() == "unknown"); // 返回实际值，不是默认值

    // 访问不存在的 key 会自动创建 null 成员（operator[] 的副作用）
    DOCTEST_CHECK(root["nonexistent"].isNull());
    DOCTEST_CHECK(root.isMember("nonexistent")); // operator[] 已创建该成员
}

/********************************************************************
 * 测试用例 9：复杂嵌套结构的序列化/反序列化 roundtrip
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Complex nested structures and roundtrip")
{
    Json::Value root(Json::objectValue);
    Json::Value items(Json::arrayValue);

    for (int i = 0; i < 3; ++i)
    {
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

    // 验证 size
    DOCTEST_CHECK(parsed["items"].size() == 3);
    DOCTEST_CHECK(parsed["meta"]["version"].asString() == "1.0");

    // 验证具体内容是否一致
    DOCTEST_CHECK(parsed["items"][0]["id"].asInt() == 0);
    DOCTEST_CHECK(parsed["items"][0]["name"].asString() == "n0");
    DOCTEST_CHECK(parsed["items"][1]["id"].asInt() == 1);
    DOCTEST_CHECK(parsed["items"][1]["name"].asString() == "n1");
    DOCTEST_CHECK(parsed["items"][2]["id"].asInt() == 2);
    DOCTEST_CHECK(parsed["items"][2]["name"].asString() == "n2");
}

/********************************************************************
 * 测试用例 10：Json::Path 使用示例（路径解析、默认值、make）
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
    Json::Path pIdx(".arr[1]"); // 注意：方括号前不要加点号

    DOCTEST_CHECK(pName.resolve(r).asString() == "Ann");
    DOCTEST_CHECK(pAge.resolve(r).asInt() == 40);
    DOCTEST_CHECK(pIdx.resolve(r).asInt() == 20);

    // 默认值示例：resolve(root, defaultValue)
    Json::Value def("missing");
    DOCTEST_CHECK(Json::Path(".person.missing").resolve(r, def).asString() == "missing");

    // make()：自动创建路径上的中间节点
    Json::Value root4make;
    Json::Path pmake(".a.b.c");
    Json::Value& ref = pmake.make(root4make);
    ref = 42;
    DOCTEST_CHECK(root4make["a"]["b"]["c"].asInt() == 42);

    // 路径不存在且无默认值时，resolve 返回 null Value
    DOCTEST_CHECK(Json::Path(".nonexistent.path").resolve(r).isNull());
}

/********************************************************************
 * 测试用例 11：大量数据构建与 roundtrip 完整验证
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Large structures and roundtrip sanity")
{
    Json::Value big(Json::arrayValue);
    const int N = 1000; // 适中数量，避免测试超时
    for (int i = 0; i < N; ++i)
    {
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

    // 验证首尾及中间元素的具体内容
    DOCTEST_CHECK(parsed[0]["index"].asInt() == 0);
    DOCTEST_CHECK(parsed[0]["text"].asString() == "entry_0");
    DOCTEST_CHECK(parsed[N - 1]["index"].asInt() == N - 1);
    DOCTEST_CHECK(parsed[N - 1]["text"].asString() == "entry_" + std::to_string(N - 1));
    DOCTEST_CHECK(parsed[500]["index"].asInt() == 500);
    DOCTEST_CHECK(parsed[500]["text"].asString() == "entry_500");
}

/********************************************************************
 * 测试用例 12：Value 比较（operator==, operator!=）
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Value comparison (operator== and operator!=)")
{
    // 相同值比较
    DOCTEST_CHECK(Json::Value(42) == Json::Value(42));
    DOCTEST_CHECK(Json::Value(3.14) == Json::Value(3.14));
    DOCTEST_CHECK(Json::Value("abc") == Json::Value("abc"));
    DOCTEST_CHECK(Json::Value(true) == Json::Value(true));

    // 不同值比较
    DOCTEST_CHECK(Json::Value(42) != Json::Value(43));
    DOCTEST_CHECK(Json::Value("abc") != Json::Value("def"));
    DOCTEST_CHECK(Json::Value(true) != Json::Value(false));

    // 不同类型比较
    DOCTEST_CHECK(Json::Value(42) != Json::Value("42"));
    DOCTEST_CHECK(Json::Value(1) != Json::Value(true));

    // 数组比较
    Json::Value a1(Json::arrayValue);
    a1.append(1); a1.append(2);
    Json::Value a2(Json::arrayValue);
    a2.append(1); a2.append(2);
    Json::Value a3(Json::arrayValue);
    a3.append(1); a3.append(3);
    DOCTEST_CHECK(a1 == a2);
    DOCTEST_CHECK(a1 != a3);

    // 对象比较
    Json::Value o1(Json::objectValue);
    o1["x"] = 1; o1["y"] = 2;
    Json::Value o2(Json::objectValue);
    o2["x"] = 1; o2["y"] = 2;
    Json::Value o3(Json::objectValue);
    o3["x"] = 1; o3["y"] = 3;
    DOCTEST_CHECK(o1 == o2);
    DOCTEST_CHECK(o1 != o3);
}

/********************************************************************
 * 测试用例 13：Value 拷贝构造、赋值与 swap
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Value copy, assignment and swap")
{
    // 拷贝构造
    Json::Value original(42);
    Json::Value copied(original);
    DOCTEST_CHECK(copied.asInt() == 42);
    DOCTEST_CHECK(copied == original);

    // 修改拷贝不影响原值
    copied = 100;
    DOCTEST_CHECK(original.asInt() == 42);
    DOCTEST_CHECK(copied.asInt() == 100);

    // 赋值运算符
    Json::Value assigned;
    assigned = original;
    DOCTEST_CHECK(assigned.asInt() == 42);
    DOCTEST_CHECK(assigned == original);

    // swap
    Json::Value a(1);
    Json::Value b("hello");
    a.swap(b);
    DOCTEST_CHECK(a.asString() == "hello");
    DOCTEST_CHECK(b.asInt() == 1);
}

/********************************************************************
 * 测试用例 14：Unicode / UTF-8 字符串的解析和序列化
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Unicode and UTF-8 strings")
{
    // 包含中文的 JSON
    const std::string json = R"({"name": "张三", "city": "北京"})";
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(json);
    DOCTEST_CHECK(Json::parseFromStream(builder, iss, &root, &errs));
    DOCTEST_CHECK(root["name"].asString() == "张三");
    DOCTEST_CHECK(root["city"].asString() == "北京");

    // 序列化包含中文的 Value
    Json::StreamWriterBuilder swb;
    swb["emitUTF8"] = true;
    swb["indentation"] = "";
    std::unique_ptr<Json::StreamWriter> writer(swb.newStreamWriter());
    std::ostringstream oss;
    writer->write(root, &oss);
    std::string out = oss.str();
    DOCTEST_CHECK(out.find("张三") != std::string::npos);

    // Unicode 转义序列 \uXXXX
    const std::string jsonEscape = R"({"emoji": "\u0041"})"; // \u0041 = 'A'
    Json::Value root2;
    std::istringstream iss2(jsonEscape);
    DOCTEST_CHECK(Json::parseFromStream(builder, iss2, &root2, &errs));
    DOCTEST_CHECK(root2["emoji"].asString() == "A");
}

/********************************************************************
 * 测试用例 15：边界值测试（大整数、特殊浮点数）
 ********************************************************************/
DOCTEST_TEST_CASE("JsonCpp - Boundary values")
{
    // 最大 Int 值
    Json::Value maxInt(std::numeric_limits<Json::Int>::max());
    DOCTEST_CHECK(maxInt.isInt());
    DOCTEST_CHECK(maxInt.asInt() == std::numeric_limits<Json::Int>::max());

    // 最小 Int 值
    Json::Value minInt(std::numeric_limits<Json::Int>::min());
    DOCTEST_CHECK(minInt.isInt());
    DOCTEST_CHECK(minInt.asInt() == std::numeric_limits<Json::Int>::min());

    // 最大 UInt 值
    Json::Value maxUInt(std::numeric_limits<Json::UInt>::max());
    DOCTEST_CHECK(maxUInt.isUInt());
    DOCTEST_CHECK(maxUInt.asUInt() == std::numeric_limits<Json::UInt>::max());

    // 空字符串
    Json::Value emptyStr("");
    DOCTEST_CHECK(emptyStr.isString());
    DOCTEST_CHECK(emptyStr.asString().empty());

    // 空数组
    Json::Value emptyArr(Json::arrayValue);
    DOCTEST_CHECK(emptyArr.isArray());
    DOCTEST_CHECK(emptyArr.size() == 0);
    DOCTEST_CHECK(emptyArr.empty());

    // 空对象
    Json::Value emptyObj(Json::objectValue);
    DOCTEST_CHECK(emptyObj.isObject());
    DOCTEST_CHECK(emptyObj.size() == 0);
    DOCTEST_CHECK(emptyObj.empty());
}