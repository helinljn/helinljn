#include "doctest.h"
#include "json/json.h"
#include <iostream>
#include <sstream>
#include <string>

DOCTEST_TEST_SUITE("JsonCpp")
{
    DOCTEST_TEST_CASE("BasicValueCreation")
    {
        // 测试各种类型的 JSON 值创建
        Json::Value nullValue; // 默认创建 null
        DOCTEST_CHECK(nullValue.isNull());
        DOCTEST_CHECK(nullValue.asString() == "");

        Json::Value boolValue(true);
        DOCTEST_CHECK(boolValue.isBool());
        DOCTEST_CHECK(boolValue.asBool() == true);

        Json::Value intValue(42);
        DOCTEST_CHECK(intValue.isInt());
        DOCTEST_CHECK(intValue.asInt() == 42);

        Json::Value doubleValue(3.14159);
        DOCTEST_CHECK(doubleValue.isDouble());
        DOCTEST_CHECK(doubleValue.asDouble() == doctest::Approx(3.14159).epsilon(1e-5));

        Json::Value stringValue("Hello, JsonCpp!");
        DOCTEST_CHECK(stringValue.isString());
        DOCTEST_CHECK(stringValue.asString() == "Hello, JsonCpp!");

        Json::Value arrayValue(Json::arrayValue);
        DOCTEST_CHECK(arrayValue.isArray());
        DOCTEST_CHECK(arrayValue.empty());

        Json::Value objectValue(Json::objectValue);
        DOCTEST_CHECK(objectValue.isObject());
        DOCTEST_CHECK(objectValue.empty());
    }

    DOCTEST_TEST_CASE("ArrayOperations")
    {
        Json::Value array(Json::arrayValue);

        // 添加元素
        array.append(1);
        array.append("test");
        array.append(true);
        array.append(3.14);

        DOCTEST_CHECK(array.size() == 4);
        DOCTEST_CHECK(array[0].asInt() == 1);
        DOCTEST_CHECK(array[1].asString() == "test");
        DOCTEST_CHECK(array[2].asBool() == true);
        DOCTEST_CHECK(array[3].asDouble() == doctest::Approx(3.14).epsilon(1e-4));

        // 测试数组索引
        array[4] = "new element";
        DOCTEST_CHECK(array.size() == 5);
        DOCTEST_CHECK(array[4].asString() == "new element");

        // 测试数组遍历
        int count = 0;
        for (Json::Value::iterator it = array.begin(); it != array.end(); ++it)
        {
            count++;
        }
        DOCTEST_CHECK(count == 5);

        // 测试数组清空
        array.clear();
        DOCTEST_CHECK(array.empty());
    }

    DOCTEST_TEST_CASE("ObjectOperations")
    {
        Json::Value object(Json::objectValue);

        // 添加成员
        object["name"] = "John";
        object["age"] = 30;
        object["isStudent"] = false;
        object["grade"] = 95.5;

        DOCTEST_CHECK(object.isMember("name"));
        DOCTEST_CHECK(object["name"].asString() == "John");
        DOCTEST_CHECK(object["age"].asInt() == 30);

        // 测试成员检查
        DOCTEST_CHECK(object.isMember("name"));
        DOCTEST_CHECK(!object.isMember("non_existent"));

        // 测试获取成员名称
        Json::Value::Members members = object.getMemberNames();
        DOCTEST_CHECK(members.size() == 4);

        // 测试删除成员
        object.removeMember("isStudent");
        DOCTEST_CHECK(!object.isMember("isStudent"));

        // 测试嵌套对象
        Json::Value address(Json::objectValue);
        address["street"] = "123 Main St";
        address["city"] = "New York";
        object["address"] = address;

        DOCTEST_CHECK(object["address"]["city"].asString() == "New York");
    }

    DOCTEST_TEST_CASE("JsonParsing")
    {
        // 测试从字符串解析 JSON
        std::string jsonStr = R"({
            "name": "John",
            "age": 30,
            "isStudent": false,
            "grades": [95, 87, 91],
            "address": {
                "street": "123 Main St",
                "city": "New York"
            }
        })";

        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;

        std::istringstream stream(jsonStr);
        DOCTEST_CHECK(Json::parseFromStream(builder, stream, &root, &errs));
        DOCTEST_CHECK(errs.empty());

        DOCTEST_CHECK(root["name"].asString() == "John");
        DOCTEST_CHECK(root["age"].asInt() == 30);
        DOCTEST_CHECK(!root["isStudent"].asBool());
        DOCTEST_CHECK(root["grades"].isArray());
        DOCTEST_CHECK(root["grades"].size() == 3);
        DOCTEST_CHECK(root["grades"][0].asInt() == 95);
        DOCTEST_CHECK(root["address"]["city"].asString() == "New York");

        // 测试解析错误
        std::string invalidJson = R"({"name": "John", "age": )";
        Json::Value invalidRoot;
        std::istringstream invalidStream(invalidJson);
        DOCTEST_CHECK(!Json::parseFromStream(builder, invalidStream, &invalidRoot, &errs));
        DOCTEST_CHECK(!errs.empty());
    }

    DOCTEST_TEST_CASE("JsonWriting")
    {
        // 测试创建 JSON 对象
        Json::Value root(Json::objectValue);
        root["name"] = "John";
        root["age"] = 30;
        root["isStudent"] = false;

        // 测试快速写入（无格式化）
        Json::FastWriter fastWriter;
        std::string fastStr = fastWriter.write(root);
        DOCTEST_CHECK(!fastStr.empty());

        // 测试美化写入（带格式化）
        Json::StyledWriter styledWriter;
        std::string styledStr = styledWriter.write(root);
        DOCTEST_CHECK(!styledStr.empty());

        // 测试 StreamWriterBuilder
        Json::StreamWriterBuilder streamBuilder;
        streamBuilder["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(streamBuilder.newStreamWriter());
        std::ostringstream oss;
        writer->write(root, &oss);
        std::string streamStr = oss.str();
        DOCTEST_CHECK(!streamStr.empty());

        // 测试写入到标准输出
        writer->write(root, &std::cout);
        std::cout << std::endl;
    }

    DOCTEST_TEST_CASE("TypeConversion")
    {
        Json::Value value;

        // 测试数字转换
        value = 42;
        DOCTEST_CHECK(value.asInt() == 42);
        DOCTEST_CHECK(value.asDouble() == 42.0);
        DOCTEST_CHECK(value.asString() == "42");

        // 测试字符串转换
        value = "123";
        DOCTEST_CHECK(value.isString());
        // 注意：JsonCpp 不会自动将字符串转换为数字
        // 当值是字符串类型时，asInt() 和 asDouble() 会抛出异常
        DOCTEST_CHECK_THROWS_AS(value.asInt(), Json::LogicError);
        DOCTEST_CHECK_THROWS_AS(value.asDouble(), Json::LogicError);
        // 测试字符串相关的转换
        DOCTEST_CHECK(value.asString() == "123");

        // 测试数字类型的转换
        value = 123;
        DOCTEST_CHECK(value.isInt());
        DOCTEST_CHECK(value.asInt() == 123);
        DOCTEST_CHECK(value.asDouble() == 123.0);
        DOCTEST_CHECK(value.asString() == "123");

        // 测试布尔转换
        value = true;
        DOCTEST_CHECK(value.asBool() == true);
        DOCTEST_CHECK(value.asInt() == 1);
        DOCTEST_CHECK(value.asString() == "true");

        // 测试空值转换
        value = Json::Value();
        DOCTEST_CHECK(value.isNull());

        // 注意：JsonCpp 中空值的转换行为可能因版本而异
        // 有些版本会抛出异常，有些版本会返回默认值
        DOCTEST_CHECK(value.asInt() == 0);
        DOCTEST_CHECK(value.asUInt() == 0);
        DOCTEST_CHECK(value.asInt64() == 0);
        DOCTEST_CHECK(value.asUInt64() == 0);
        DOCTEST_CHECK(value.asBool() == false);
        DOCTEST_CHECK(value.asDouble() == 0.0);
        DOCTEST_CHECK(value.asString() == "");
    }

    DOCTEST_TEST_CASE("SpecialFeatures")
    {
        // 测试注释支持
        std::string jsonWithComments = R"(
            {
                // This is a comment
                "name": "John", /* Another comment */
                "age": 30
            }
        )";

        Json::Value root;
        Json::CharReaderBuilder builder;
        builder["allowComments"] = true;
        std::string errs;

        std::istringstream stream(jsonWithComments);
        DOCTEST_CHECK(Json::parseFromStream(builder, stream, &root, &errs));
        DOCTEST_CHECK(root["name"].asString() == "John");

        // 测试尾随逗号
        std::string jsonWithTrailingComma = R"({
            "name": "John",
            "age": 30,
        })";

        Json::Value root2;
        Json::CharReaderBuilder builder2;
        builder2["allowTrailingCommas"] = true;
        std::string errs2;

        std::istringstream stream2(jsonWithTrailingComma);
        DOCTEST_CHECK(Json::parseFromStream(builder2, stream2, &root2, &errs2));
        DOCTEST_CHECK(root2["age"].asInt() == 30);

        // 测试单引号
        std::string jsonWithSingleQuotes = R"({'name': 'John', 'age': 30})";

        Json::Value root3;
        Json::CharReaderBuilder builder3;
        builder3["allowSingleQuotes"] = true;
        std::string errs3;

        std::istringstream stream3(jsonWithSingleQuotes);
        DOCTEST_CHECK(Json::parseFromStream(builder3, stream3, &root3, &errs3));
        DOCTEST_CHECK(root3["name"].asString() == "John");
    }

    DOCTEST_TEST_CASE("ErrorHandling")
    {
        // 测试解析错误处理
        std::string invalidJson = R"({
            "name": "John",
            "age": "thirty"
        })";

        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;

        // 注意：JsonCpp 会将字符串 "thirty" 视为有效，只是类型转换会失败
        std::istringstream stream(invalidJson);
        DOCTEST_CHECK(Json::parseFromStream(builder, stream, &root, &errs));

        // 测试类型转换错误
        DOCTEST_CHECK(root["age"].isString());
        // 注意：JsonCpp 不会自动将字符串转换为数字
        // 当值是字符串类型时，asInt() 会抛出异常
        DOCTEST_CHECK_THROWS_AS(root["age"].asInt(), Json::LogicError);
        DOCTEST_CHECK_THROWS_AS(root["age"].asDouble(), Json::LogicError);

        // 测试访问不存在的成员
        // 注意：JsonCpp 中使用 [] 操作符访问不存在的成员会自动插入空值
        // 所以我们应该使用 isMember 方法来检查成员是否存在
        DOCTEST_CHECK(!root.isMember("non_existent"));
        // 测试 get 方法的默认值行为
        const Json::Value& defaultValue = root.get("non_existent", "default");
        DOCTEST_CHECK(defaultValue.asString() == "default");
    }

    DOCTEST_TEST_CASE("ComplexStructures")
    {
        // 测试复杂的嵌套结构
        Json::Value complex(Json::objectValue);

        // 添加数组
        Json::Value array(Json::arrayValue);
        array.append(1);
        array.append(2);
        array.append(3);
        complex["numbers"] = array;

        // 添加嵌套对象
        Json::Value nested(Json::objectValue);
        nested["inner1"] = "value1";
        nested["inner2"] = 123;
        complex["nested"] = nested;

        // 添加嵌套数组
        Json::Value nestedArray(Json::arrayValue);
        Json::Value item1(Json::objectValue);
        item1["name"] = "item1";
        nestedArray.append(item1);
        complex["items"] = nestedArray;

        // 验证结构
        DOCTEST_CHECK(complex["numbers"].isArray());
        DOCTEST_CHECK(complex["numbers"].size() == 3);
        DOCTEST_CHECK(complex["nested"]["inner1"].asString() == "value1");
        DOCTEST_CHECK(complex["items"][0]["name"].asString() == "item1");

        // 测试序列化和反序列化
        Json::StyledWriter writer;
        std::string jsonStr = writer.write(complex);

        Json::Value parsed;
        Json::CharReaderBuilder builder;
        std::string errs;
        std::istringstream stream(jsonStr);
        DOCTEST_CHECK(Json::parseFromStream(builder, stream, &parsed, &errs));
        DOCTEST_CHECK(parsed["nested"]["inner2"].asInt() == 123);
    }

    DOCTEST_TEST_CASE("JsonPath")
    {
        // 测试 Json::Path 功能
        Json::Value root(Json::objectValue);
        root["person"]["name"] = "John";
        root["person"]["age"] = 30;
        root["person"]["address"]["city"] = "New York";
        root["numbers"] = Json::Value(Json::arrayValue);
        root["numbers"].append(10);
        root["numbers"].append(20);
        root["numbers"].append(30);

        // 测试路径解析
        Json::Path namePath(".person.name");
        Json::Path agePath(".person.age");
        Json::Path cityPath(".person.address.city");
        Json::Path numberPath(".numbers.[1]");

        DOCTEST_CHECK(namePath.resolve(root).asString() == "John");
        DOCTEST_CHECK(agePath.resolve(root).asInt() == 30);
        DOCTEST_CHECK(cityPath.resolve(root).asString() == "New York");
        DOCTEST_CHECK(numberPath.resolve(root).asInt() == 20);

        // 测试默认值
        Json::Path nonExistentPath(".person.non_existent");
        Json::Value defaultValue("default");
        DOCTEST_CHECK(nonExistentPath.resolve(root, defaultValue).asString() == "default");
    }

    DOCTEST_TEST_CASE("PerformanceConsiderations")
    {
        // 测试大型 JSON 处理
        Json::Value largeArray(Json::arrayValue);

        // 添加 1000 个元素
        for (int i = 0; i < 1000; i++)
        {
            Json::Value item(Json::objectValue);
            item["id"] = i;
            item["name"] = "Item " + std::to_string(i);
            item["value"] = i * 10.5;
            largeArray.append(item);
        }

        DOCTEST_CHECK(largeArray.size() == 1000);
        DOCTEST_CHECK(largeArray[999]["id"].asInt() == 999);

        // 测试序列化速度（这里只是测试功能，不做性能基准）
        Json::FastWriter fastWriter;
        std::string jsonStr = fastWriter.write(largeArray);
        DOCTEST_CHECK(!jsonStr.empty());

        // 测试反序列化
        Json::Value parsed;
        Json::CharReaderBuilder builder;
        std::string errs;
        std::istringstream stream(jsonStr);
        DOCTEST_CHECK(Json::parseFromStream(builder, stream, &parsed, &errs));
        DOCTEST_CHECK(parsed.size() == 1000);
    }
}