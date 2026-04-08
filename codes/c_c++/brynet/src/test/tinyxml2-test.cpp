#include "doctest.h"
#include "tinyxml2.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>

using namespace tinyxml2;

// 辅助函数：将 XMLError 转换为 bool，用于 doctest 断言
// 因为 doctest 对 enum 类型的表达式分解支持有限，使用此函数避免编译警告
inline bool xml_ok(XMLError err)
{
    return err == XML_SUCCESS;
}

// RAII 临时文件清理辅助类，确保测试失败时也能删除临时文件
struct TempFileGuard
{
    std::string path;
    explicit TempFileGuard(const char* p) : path(p) {}
    ~TempFileGuard() { std::remove(path.c_str()); }
};

/**
 * 测试用例 1：从字符串解析 XML 并访问根元素
 *
 * 功能说明：
 * - 演示如何使用 XMLDocument::Parse() 从内存中的字符串解析 XML
 * - 演示如何使用 RootElement() 获取 XML 文档的根元素
 * - 演示如何使用 Name() 获取元素的标签名
 * - 演示如何使用 Attribute() 读取元素的属性值
 *
 * 测试步骤：
 * 1. 创建 XMLDocument 对象
 * 2. 调用 Parse() 解析 XML 字符串
 * 3. 验证解析成功（返回 XML_SUCCESS）
 * 4. 获取根元素并验证其标签名为 "root"
 * 5. 读取根元素的 "version" 属性并验证其值为 "1.0"
 */
DOCTEST_TEST_CASE("XMLDocument - 从字符串解析 XML 并访问根元素")
{
    // 准备测试用的 XML 字符串
    const char* xmlStr = R"(<?xml version="1.0"?>
<root version="1.0">
    <child>Hello</child>
</root>)";

    // 创建 XML 文档对象
    XMLDocument doc;

    // 从字符串解析 XML，Parse() 返回 XMLError 枚举值
    // XML_SUCCESS 表示解析成功
    XMLError err = doc.Parse(xmlStr);
    DOCTEST_CHECK(xml_ok(err));

    // 获取根元素（文档的第一个元素节点）
    XMLElement* root = doc.RootElement();
    DOCTEST_REQUIRE(root != nullptr);

    // 验证根元素的标签名
    DOCTEST_CHECK(std::strcmp(root->Name(), "root") == 0);

    // 读取根元素的 "version" 属性
    // Attribute() 返回属性值字符串，如果属性不存在则返回 nullptr
    const char* version = root->Attribute("version");
    DOCTEST_REQUIRE(version != nullptr);
    DOCTEST_CHECK(std::strcmp(version, "1.0") == 0);
}

/**
 * 测试用例 2：解析无效的 XML 并检测错误
 *
 * 功能说明：
 * - 演示如何检测 XML 解析错误
 * - 演示如何使用 Error() 判断是否有错误
 * - 演示如何使用 ErrorID() 获取错误代码
 * - 演示如何使用 ErrorStr() 获取错误描述信息
 * - 演示如何使用 ErrorLineNum() 获取错误行号
 *
 * 测试步骤：
 * 1. 创建 XMLDocument 对象
 * 2. 尝试解析一个格式错误的 XML（缺少闭合标签）
 * 3. 验证 Parse() 返回非 XML_SUCCESS 的错误码
 * 4. 使用 Error() 方法确认文档处于错误状态
 * 5. 使用 ErrorID() 获取具体的错误类型（XML_ERROR_MISMATCHED_ELEMENT）
 * 6. 使用 ErrorStr() 获取人类可读的错误描述
 * 7. 使用 ErrorLineNum() 验证错误行号大于 0
 */
DOCTEST_TEST_CASE("XMLDocument - 解析无效 XML 并检测错误")
{
    // 准备一个格式错误的 XML 字符串（<child> 标签没有闭合）
    const char* badXml = "<root><child>text</root>";

    XMLDocument doc;

    // 尝试解析错误的 XML
    XMLError err = doc.Parse(badXml);

    // 验证解析失败（返回值不是 XML_SUCCESS）
    DOCTEST_CHECK(!xml_ok(err));

    // Error() 方法返回 true 表示文档有错误
    DOCTEST_CHECK(doc.Error());

    // ErrorID() 返回具体的错误类型枚举值，验证为标签不匹配
    DOCTEST_CHECK(doc.ErrorID() == XML_ERROR_MISMATCHED_ELEMENT);

    // ErrorStr() 返回详细的错误描述字符串，用于调试
    const char* errStr = doc.ErrorStr();
    DOCTEST_CHECK(errStr != nullptr);
    DOCTEST_CHECK(std::strlen(errStr) > 0);

    // ErrorLineNum() 返回错误发生的行号
    DOCTEST_CHECK(doc.ErrorLineNum() > 0);
}

/**
 * 测试用例 3：动态创建 XML 节点（元素、注释、文本）
 *
 * 功能说明：
 * - 演示如何使用 NewElement() 创建新的元素节点
 * - 演示如何使用 NewComment() 创建注释节点
 * - 演示如何使用 NewText() 创建文本节点
 * - 演示如何使用 InsertEndChild() 将节点插入到文档树中
 * - 演示如何构建完整的 XML 文档结构
 *
 * 测试步骤：
 * 1. 创建空的 XMLDocument 对象
 * 2. 创建根元素 "config"
 * 3. 创建注释节点并插入到根元素
 * 4. 创建子元素 "setting" 并设置属性
 * 5. 创建文本节点并插入到子元素中
 * 6. 将根元素插入到文档
 * 7. 验证文档结构的正确性
 */
DOCTEST_TEST_CASE("XMLDocument - 动态创建元素、注释和文本节点")
{
    XMLDocument doc;

    // 创建根元素 "config"
    // NewElement() 创建的节点归 XMLDocument 管理，不需要手动 delete
    XMLElement* root = doc.NewElement("config");
    DOCTEST_REQUIRE(root != nullptr);

    // 创建注释节点 "<!-- Configuration file -->"
    XMLComment* comment = doc.NewComment(" Configuration file ");
    DOCTEST_REQUIRE(comment != nullptr);

    // 将注释插入到根元素的子节点列表末尾
    root->InsertEndChild(comment);

    // 创建子元素 "setting"
    XMLElement* setting = doc.NewElement("setting");
    DOCTEST_REQUIRE(setting != nullptr);

    // 为 "setting" 元素设置属性 name="timeout"
    setting->SetAttribute("name", "timeout");

    // 创建文本节点，内容为 "30"
    XMLText* text = doc.NewText("30");
    DOCTEST_REQUIRE(text != nullptr);

    // 将文本节点插入到 "setting" 元素中
    setting->InsertEndChild(text);

    // 将 "setting" 元素插入到根元素
    root->InsertEndChild(setting);

    // 将根元素插入到文档（成为文档的根节点）
    doc.InsertEndChild(root);

    // 验证文档结构
    XMLElement* docRoot = doc.RootElement();
    DOCTEST_REQUIRE(docRoot != nullptr);
    DOCTEST_CHECK(std::strcmp(docRoot->Name(), "config") == 0);

    // 验证第一个子节点是注释
    XMLNode* firstChild = docRoot->FirstChild();
    DOCTEST_REQUIRE(firstChild != nullptr);
    DOCTEST_CHECK(firstChild->ToComment() != nullptr);

    // 验证第一个子元素是 "setting"
    XMLElement* settingElem = docRoot->FirstChildElement("setting");
    DOCTEST_REQUIRE(settingElem != nullptr);
    DOCTEST_CHECK(std::strcmp(settingElem->Attribute("name"), "timeout") == 0);

    // 验证 "setting" 元素的文本内容
    const char* settingText = settingElem->GetText();
    DOCTEST_REQUIRE(settingText != nullptr);
    DOCTEST_CHECK(std::strcmp(settingText, "30") == 0);
}

/**
 * 测试用例 4：保存 XML 到文件并重新加载
 *
 * 功能说明：
 * - 演示如何使用 SaveFile() 将 XML 文档保存到文件
 * - 演示如何使用 LoadFile() 从文件加载 XML 文档
 * - 演示文件读写的完整流程（往返测试）
 * - 验证保存和加载后数据的一致性
 * - 使用 RAII 保证临时文件清理
 *
 * 测试步骤：
 * 1. 创建一个包含数据的 XML 文档
 * 2. 使用 SaveFile() 保存到临时文件
 * 3. 创建新的 XMLDocument 对象
 * 4. 使用 LoadFile() 从文件加载
 * 5. 验证加载后的数据与原始数据一致
 * 6. RAII 自动清理临时文件
 */
DOCTEST_TEST_CASE("XMLDocument - 保存到文件并重新加载")
{
    const char* filename = "test_output.xml";
    TempFileGuard guard(filename);

    // 第一步：创建并保存 XML 文档
    {
        XMLDocument doc;
        XMLElement* root = doc.NewElement("data");
        root->SetAttribute("id", "123");

        XMLElement* item = doc.NewElement("item");
        item->SetText("test value");
        root->InsertEndChild(item);

        doc.InsertEndChild(root);

        // 保存到文件，返回 XML_SUCCESS 表示成功
        XMLError saveErr = doc.SaveFile(filename);
        DOCTEST_CHECK(xml_ok(saveErr));
    }

    // 第二步：从文件加载并验证
    {
        XMLDocument doc;

        // 从文件加载 XML
        XMLError loadErr = doc.LoadFile(filename);
        DOCTEST_CHECK(xml_ok(loadErr));

        // 验证根元素
        XMLElement* root = doc.RootElement();
        DOCTEST_REQUIRE(root != nullptr);
        DOCTEST_CHECK(std::strcmp(root->Name(), "data") == 0);

        // 验证属性
        int id = root->IntAttribute("id");
        DOCTEST_CHECK(id == 123);

        // 验证子元素
        XMLElement* item = root->FirstChildElement("item");
        DOCTEST_REQUIRE(item != nullptr);
        const char* text = item->GetText();
        DOCTEST_REQUIRE(text != nullptr);
        DOCTEST_CHECK(std::strcmp(text, "test value") == 0);
    }
}

/**
 * 测试用例 5：读取各种类型的属性
 *
 * 功能说明：
 * - 演示如何读取字符串类型的属性
 * - 演示如何读取整型属性（IntAttribute）
 * - 演示如何读取布尔型属性（BoolAttribute）
 * - 演示如何使用 QueryIntAttribute 进行安全的属性查询
 * - 演示属性不存在时的默认值处理
 * - 演示 QueryBoolAttribute / QueryDoubleAttribute / QueryFloatAttribute
 * - 演示 BoolAttribute 默认值
 * - 演示 FindAttribute 查找属性
 *
 * 测试步骤：
 * 1. 解析包含多种类型属性的 XML
 * 2. 使用 Attribute() 读取字符串属性
 * 3. 使用 IntAttribute() 读取整型属性
 * 4. 使用 BoolAttribute() 读取布尔型属性
 * 5. 使用 QueryIntAttribute() 进行带错误检查的属性读取
 * 6. 测试不存在的属性返回默认值
 * 7. 测试 QueryBoolAttribute / QueryDoubleAttribute / QueryFloatAttribute
 * 8. 测试 BoolAttribute 不存在属性的默认值
 * 9. 测试 FindAttribute
 */
DOCTEST_TEST_CASE("XMLElement - 读取各种类型的属性")
{
    // XML 包含字符串、整型、布尔型、浮点型属性
    const char* xmlStr = R"(<config name="app" port="8080" enabled="true" ratio="0.5"/>)";

    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xmlStr)));

    XMLElement* config = doc.RootElement();
    DOCTEST_REQUIRE(config != nullptr);

    // 读取字符串属性 "name"
    const char* name = config->Attribute("name");
    DOCTEST_REQUIRE(name != nullptr);
    DOCTEST_CHECK(std::strcmp(name, "app") == 0);

    // 读取整型属性 "port"，IntAttribute() 会自动将字符串转换为整数
    int port = config->IntAttribute("port");
    DOCTEST_CHECK(port == 8080);

    // 读取布尔型属性 "enabled"，BoolAttribute() 会将 "true"/"false" 转换为 bool
    bool enabled = config->BoolAttribute("enabled");
    DOCTEST_CHECK(enabled == true);

    // 使用 QueryIntAttribute 进行安全查询（带错误检查）
    int portValue = 0;
    XMLError err = config->QueryIntAttribute("port", &portValue);
    DOCTEST_CHECK(xml_ok(err));
    DOCTEST_CHECK(portValue == 8080);

    // 查询不存在的属性，IntAttribute 返回默认值 0
    int missing = config->IntAttribute("notexist");
    DOCTEST_CHECK(missing == 0);

    // 可以指定默认值
    int missingWithDefault = config->IntAttribute("notexist", 9999);
    DOCTEST_CHECK(missingWithDefault == 9999);

    // BoolAttribute 不存在的属性返回默认值 false
    bool boolDefault = config->BoolAttribute("notexist");
    DOCTEST_CHECK(boolDefault == false);

    // BoolAttribute 可以指定默认值
    bool boolWithDefault = config->BoolAttribute("notexist", true);
    DOCTEST_CHECK(boolWithDefault == true);

    // QueryBoolAttribute 安全查询
    bool boolVal = false;
    XMLError boolErr = config->QueryBoolAttribute("enabled", &boolVal);
    DOCTEST_CHECK(xml_ok(boolErr));
    DOCTEST_CHECK(boolVal == true);

    // QueryBoolAttribute 查询不存在的属性返回 XML_NO_ATTRIBUTE
    bool dummy = false;
    XMLError missingBoolErr = config->QueryBoolAttribute("notexist", &dummy);
    DOCTEST_CHECK(missingBoolErr == XML_NO_ATTRIBUTE);

    // QueryDoubleAttribute 安全查询
    double doubleVal = 0.0;
    XMLError doubleErr = config->QueryDoubleAttribute("ratio", &doubleVal);
    DOCTEST_CHECK(xml_ok(doubleErr));
    DOCTEST_CHECK(std::fabs(doubleVal - 0.5) < 1e-9);

    // QueryFloatAttribute 安全查询
    float floatVal = 0.0f;
    XMLError floatErr = config->QueryFloatAttribute("ratio", &floatVal);
    DOCTEST_CHECK(xml_ok(floatErr));
    DOCTEST_CHECK(std::fabs(floatVal - 0.5f) < 1e-6f);

    // FindAttribute 查找属性
    const XMLAttribute* foundAttr = config->FindAttribute("name");
    DOCTEST_REQUIRE(foundAttr != nullptr);
    DOCTEST_CHECK(std::strcmp(foundAttr->Value(), "app") == 0);

    // FindAttribute 查找不存在的属性返回 nullptr
    const XMLAttribute* notFound = config->FindAttribute("notexist");
    DOCTEST_CHECK(notFound == nullptr);
}

/**
 * 测试用例 6：设置和删除属性
 *
 * 功能说明：
 * - 演示如何使用 SetAttribute() 设置各种类型的属性（string, int, bool, int64, double, float, unsigned）
 * - 演示如何使用 DeleteAttribute() 删除属性
 * - 演示属性的动态修改
 *
 * 测试步骤：
 * 1. 创建一个元素
 * 2. 使用 SetAttribute() 设置多种类型的属性
 * 3. 验证属性设置成功
 * 4. 使用 DeleteAttribute() 删除属性
 * 5. 验证属性已被删除
 */
DOCTEST_TEST_CASE("XMLElement - 设置和删除属性")
{
    XMLDocument doc;
    XMLElement* elem = doc.NewElement("server");

    // 设置字符串属性
    elem->SetAttribute("host", "localhost");

    // 设置整型属性
    elem->SetAttribute("port", 3000);

    // 设置布尔型属性
    elem->SetAttribute("ssl", true);

    // 设置 int64 属性
    elem->SetAttribute("bigid", (int64_t)922337203685477LL);

    // 设置 double 属性
    elem->SetAttribute("score", 98.5);

    // 设置 float 属性
    elem->SetAttribute("ratio", 0.75f);

    // 设置 unsigned 属性
    elem->SetAttribute("count", (unsigned)42);

    // 验证属性值
    DOCTEST_CHECK(std::strcmp(elem->Attribute("host"), "localhost") == 0);
    DOCTEST_CHECK(elem->IntAttribute("port") == 3000);
    DOCTEST_CHECK(elem->BoolAttribute("ssl") == true);
    DOCTEST_CHECK(elem->Int64Attribute("bigid") == (int64_t)922337203685477LL);
    DOCTEST_CHECK(std::fabs(elem->DoubleAttribute("score") - 98.5) < 1e-9);
    DOCTEST_CHECK(std::fabs(elem->FloatAttribute("ratio") - 0.75f) < 1e-6f);
    DOCTEST_CHECK(elem->UnsignedAttribute("count") == 42u);

    // 删除 "ssl" 属性
    elem->DeleteAttribute("ssl");

    // 验证属性已被删除（返回 nullptr）
    const char* ssl = elem->Attribute("ssl");
    DOCTEST_CHECK(ssl == nullptr);

    // 修改现有属性
    elem->SetAttribute("port", 8080);
    DOCTEST_CHECK(elem->IntAttribute("port") == 8080);
}

/**
 * 测试用例 7：获取和设置元素文本内容
 *
 * 功能说明：
 * - 演示如何使用 GetText() 获取元素的文本内容
 * - 演示如何使用 SetText() 设置元素的文本内容（string, int, unsigned, int64, bool, double, float）
 * - 演示 QueryIntText() 进行类型安全的文本查询
 * - 演示 QueryBoolText / QueryDoubleText / QueryFloatText
 * - 演示 IntText / DoubleText / BoolText 等便捷方法
 * - 演示 GetText() 对无文本元素返回 nullptr
 *
 * 测试步骤：
 * 1. 解析包含文本内容的 XML
 * 2. 使用 GetText() 读取文本
 * 3. 使用 SetText() 修改文本（字符串和多种数值类型）
 * 4. 使用 QueryIntText() 查询整型文本
 * 5. 使用各种 Query*Text 方法
 * 6. 使用 IntText / DoubleText 等便捷方法
 * 7. 测试无文本元素的 GetText() 返回 nullptr
 */
DOCTEST_TEST_CASE("XMLElement - 获取和设置元素文本内容")
{
    const char* xmlStr = R"(<data>
    <message>Hello World</message>
    <count>42</count>
    <flag>true</flag>
    <pi>3.14</pi>
    <empty/>
</data>)";

    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xmlStr)));

    XMLElement* root = doc.RootElement();
    DOCTEST_REQUIRE(root != nullptr);

    // 获取 "message" 元素的文本内容
    XMLElement* message = root->FirstChildElement("message");
    DOCTEST_REQUIRE(message != nullptr);
    const char* text = message->GetText();
    DOCTEST_REQUIRE(text != nullptr);
    DOCTEST_CHECK(std::strcmp(text, "Hello World") == 0);

    // 修改 "message" 元素的文本内容
    message->SetText("New Message");
    DOCTEST_CHECK(std::strcmp(message->GetText(), "New Message") == 0);

    // 获取 "count" 元素的整型文本
    XMLElement* count = root->FirstChildElement("count");
    DOCTEST_REQUIRE(count != nullptr);

    // 使用 QueryIntText 安全地查询整型文本
    int countValue = 0;
    XMLError err = count->QueryIntText(&countValue);
    DOCTEST_CHECK(xml_ok(err));
    DOCTEST_CHECK(countValue == 42);

    // 使用 SetText 设置整型文本
    count->SetText(100);
    int newCount = 0;
    count->QueryIntText(&newCount);
    DOCTEST_CHECK(newCount == 100);

    // 使用 SetText 设置 int64 文本
    count->SetText((int64_t)123456789012LL);
    int64_t i64Val = 0;
    DOCTEST_CHECK(xml_ok(count->QueryInt64Text(&i64Val)));
    DOCTEST_CHECK(i64Val == (int64_t)123456789012LL);

    // 使用 SetText 设置 unsigned 文本
    count->SetText((unsigned)999);
    unsigned uVal = 0;
    DOCTEST_CHECK(xml_ok(count->QueryUnsignedText(&uVal)));
    DOCTEST_CHECK(uVal == 999u);

    // 使用 SetText 设置 bool 文本
    count->SetText(true);
    bool bVal = false;
    DOCTEST_CHECK(xml_ok(count->QueryBoolText(&bVal)));
    DOCTEST_CHECK(bVal == true);

    // 使用 SetText 设置 double 文本
    count->SetText(2.71828);
    double dVal = 0.0;
    DOCTEST_CHECK(xml_ok(count->QueryDoubleText(&dVal)));
    DOCTEST_CHECK(std::fabs(dVal - 2.71828) < 1e-9);

    // 使用 SetText 设置 float 文本
    count->SetText(1.5f);
    float fVal = 0.0f;
    DOCTEST_CHECK(xml_ok(count->QueryFloatText(&fVal)));
    DOCTEST_CHECK(std::fabs(fVal - 1.5f) < 1e-6f);

    // 使用 "flag" 元素测试 QueryBoolText
    XMLElement* flag = root->FirstChildElement("flag");
    DOCTEST_REQUIRE(flag != nullptr);
    bool flagVal = false;
    DOCTEST_CHECK(xml_ok(flag->QueryBoolText(&flagVal)));
    DOCTEST_CHECK(flagVal == true);

    // 使用 "pi" 元素测试 QueryDoubleText / QueryFloatText
    XMLElement* pi = root->FirstChildElement("pi");
    DOCTEST_REQUIRE(pi != nullptr);
    double piDouble = 0.0;
    DOCTEST_CHECK(xml_ok(pi->QueryDoubleText(&piDouble)));
    DOCTEST_CHECK(std::fabs(piDouble - 3.14) < 1e-9);

    float piFloat = 0.0f;
    DOCTEST_CHECK(xml_ok(pi->QueryFloatText(&piFloat)));
    DOCTEST_CHECK(std::fabs(piFloat - 3.14f) < 1e-5f);

    // 使用便捷方法 IntText / DoubleText / BoolText
    DOCTEST_CHECK(count->IntText() == 1);  // bool(true) 写入后读为 int = 1
    DOCTEST_CHECK(pi->DoubleText() == 3.14);
    DOCTEST_CHECK(flag->BoolText() == true);

    // 无文本元素 GetText() 返回 nullptr
    XMLElement* empty = root->FirstChildElement("empty");
    DOCTEST_REQUIRE(empty != nullptr);
    DOCTEST_CHECK(empty->GetText() == nullptr);
}

/**
 * 测试用例 8：遍历 XML 树结构（兄弟节点和子节点）
 *
 * 功能说明：
 * - 演示如何使用 FirstChildElement() 获取第一个子元素
 * - 演示如何使用 NextSiblingElement() 遍历兄弟元素
 * - 演示如何使用 LastChildElement() 获取最后一个子元素
 * - 演示如何使用 PreviousSiblingElement() 反向遍历
 * - 演示如何遍历所有同名或所有子元素
 *
 * 测试步骤：
 * 1. 解析包含多个子元素的 XML
 * 2. 使用 FirstChildElement() 获取第一个子元素
 * 3. 使用 NextSiblingElement() 遍历所有兄弟元素
 * 4. 使用 LastChildElement() 获取最后一个子元素
 * 5. 使用 PreviousSiblingElement() 反向遍历
 * 6. 统计子元素数量
 */
DOCTEST_TEST_CASE("XMLNode - 遍历树结构和兄弟元素")
{
    const char* xmlStr = R"(<list>
    <item>First</item>
    <item>Second</item>
    <item>Third</item>
</list>)";

    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xmlStr)));

    XMLElement* list = doc.RootElement();
    DOCTEST_REQUIRE(list != nullptr);

    // 获取第一个 "item" 子元素
    XMLElement* first = list->FirstChildElement("item");
    DOCTEST_REQUIRE(first != nullptr);
    DOCTEST_CHECK(std::strcmp(first->GetText(), "First") == 0);

    // 遍历所有 "item" 兄弟元素
    int itemCount = 0;
    for (XMLElement* item = list->FirstChildElement("item");
         item != nullptr;
         item = item->NextSiblingElement("item"))
    {
        itemCount++;
    }
    DOCTEST_CHECK(itemCount == 3);

    // 获取最后一个 "item" 子元素
    XMLElement* last = list->LastChildElement("item");
    DOCTEST_REQUIRE(last != nullptr);
    DOCTEST_CHECK(std::strcmp(last->GetText(), "Third") == 0);

    // 验证第一个和最后一个不是同一个元素
    DOCTEST_CHECK(first != last);

    // 使用 PreviousSiblingElement 反向遍历：从 last 到 first
    XMLElement* prev = last->PreviousSiblingElement("item");
    DOCTEST_REQUIRE(prev != nullptr);
    DOCTEST_CHECK(std::strcmp(prev->GetText(), "Second") == 0);

    XMLElement* firstAgain = prev->PreviousSiblingElement("item");
    DOCTEST_REQUIRE(firstAgain != nullptr);
    DOCTEST_CHECK(std::strcmp(firstAgain->GetText(), "First") == 0);
    DOCTEST_CHECK(firstAgain == first);

    // first 的 PreviousSiblingElement 应为 nullptr
    DOCTEST_CHECK(first->PreviousSiblingElement("item") == nullptr);
}

/**
 * 测试用例 9：插入新子元素并验证
 *
 * 功能说明：
 * - 演示如何使用 InsertNewChildElement() 快速插入新子元素
 * - 演示如何使用 InsertEndChild() 插入已创建的节点
 * - 演示插入后的元素顺序
 * - 演示如何为新插入的元素设置属性和文本
 * - 演示 InsertNewComment / InsertNewText 等便捷方法
 *
 * 测试步骤：
 * 1. 创建根元素
 * 2. 使用 InsertNewChildElement() 插入多个子元素
 * 3. 为子元素设置属性和文本内容
 * 4. 验证插入顺序和内容
 * 5. 使用 InsertNewComment / InsertNewText 便捷方法
 */
DOCTEST_TEST_CASE("XMLElement - 插入新子元素并验证")
{
    XMLDocument doc;
    XMLElement* root = doc.NewElement("users");
    doc.InsertEndChild(root);

    // 使用 InsertNewChildElement 快速插入子元素
    // 这个方法会自动创建元素并插入，返回新创建的元素指针
    XMLElement* user1 = root->InsertNewChildElement("user");
    DOCTEST_REQUIRE(user1 != nullptr);
    user1->SetAttribute("id", 1);
    user1->SetAttribute("name", "Alice");

    XMLElement* user2 = root->InsertNewChildElement("user");
    DOCTEST_REQUIRE(user2 != nullptr);
    user2->SetAttribute("id", 2);
    user2->SetAttribute("name", "Bob");

    // 验证插入的元素数量
    int userCount = 0;
    for (XMLElement* user = root->FirstChildElement("user");
         user != nullptr;
         user = user->NextSiblingElement("user"))
    {
        userCount++;
    }
    DOCTEST_CHECK(userCount == 2);

    // 验证第一个用户
    XMLElement* firstUser = root->FirstChildElement("user");
    DOCTEST_REQUIRE(firstUser != nullptr);
    DOCTEST_CHECK(firstUser->IntAttribute("id") == 1);
    DOCTEST_CHECK(std::strcmp(firstUser->Attribute("name"), "Alice") == 0);

    // 验证第二个用户
    XMLElement* secondUser = firstUser->NextSiblingElement("user");
    DOCTEST_REQUIRE(secondUser != nullptr);
    DOCTEST_CHECK(secondUser->IntAttribute("id") == 2);
    DOCTEST_CHECK(std::strcmp(secondUser->Attribute("name"), "Bob") == 0);

    // 使用 InsertNewComment 便捷方法
    XMLComment* cmt = root->InsertNewComment(" end of users ");
    DOCTEST_REQUIRE(cmt != nullptr);

    // 使用 InsertNewText 便捷方法
    XMLElement* note = root->InsertNewChildElement("note");
    XMLText* txt = note->InsertNewText("some text");
    DOCTEST_REQUIRE(txt != nullptr);
    DOCTEST_CHECK(std::strcmp(note->GetText(), "some text") == 0);

    // InsertNewDeclaration / InsertNewUnknown 是 XMLElement 的方法
    // 使用 root 元素调用
    XMLDeclaration* decl = root->InsertNewDeclaration("xml version=\"1.0\"");
    DOCTEST_REQUIRE(decl != nullptr);

    XMLUnknown* unk = root->InsertNewUnknown("!DOCTYPE html");
    DOCTEST_REQUIRE(unk != nullptr);
}

/**
 * 测试用例 10：XMLPrinter - 序列化输出测试
 *
 * 功能说明：
 * - 演示如何使用 XMLPrinter 将 XMLDocument 序列化为字符串
 * - 验证序列化结果中包含元素和声明
 */
DOCTEST_TEST_CASE("XMLPrinter - 序列化输出")
{
    XMLDocument doc;
    // 创建声明并插入到文档开头
    XMLDeclaration* decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    doc.InsertFirstChild(decl);

    XMLElement* root = doc.NewElement("root");
    doc.InsertEndChild(root);

    XMLElement* child = doc.NewElement("child");
    child->SetText("val");
    root->InsertEndChild(child);

    // 使用 XMLPrinter 序列化到内存
    XMLPrinter printer;
    doc.Print(&printer);

    const char* out = printer.CStr();
    DOCTEST_REQUIRE(out != nullptr);

    // 验证序列化字符串中包含预期片段
    DOCTEST_CHECK(std::strstr(out, "<?xml") != nullptr);
    DOCTEST_CHECK(std::strstr(out, "<root>") != nullptr);
    DOCTEST_CHECK(std::strstr(out, "<child>val</child>") != nullptr);
}

/**
 * 测试用例 11：XMLHandle - 安全链式访问测试
 *
 * 功能说明：
 * - 演示使用 XMLHandle 进行链式、空安全的元素访问
 * - 避免频繁的空指针检查，便于安全地取得深层节点
 * - 演示链式访问不存在的节点时安全返回 nullptr
 */
DOCTEST_TEST_CASE("XMLHandle - 安全链式访问")
{
    const char* xml = R"(<a><b><c id="5">hello</c></b></a>)";
    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xml)));

    XMLHandle docHandle(doc);
    XMLElement* c = docHandle.FirstChildElement("a").FirstChildElement("b").FirstChildElement("c").ToElement();
    DOCTEST_REQUIRE(c != nullptr);
    DOCTEST_CHECK(std::strcmp(c->Attribute("id"), "5") == 0);
    DOCTEST_CHECK(std::strcmp(c->GetText(), "hello") == 0);

    // 链式访问不存在的节点，安全返回 nullptr
    XMLElement* notExist = docHandle.FirstChildElement("x").FirstChildElement("y").ToElement();
    DOCTEST_CHECK(notExist == nullptr);
}

/**
 * 测试用例 12：XMLDeclaration - 创建与读取声明
 *
 * 功能说明：
 * - 演示如何创建 XMLDeclaration 并插入到文档
 * - 演示如何读取文档头部的声明节点
 */
DOCTEST_TEST_CASE("XMLDeclaration - 创建与读取声明")
{
    XMLDocument doc;
    XMLDeclaration* dec = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    doc.InsertFirstChild(dec);

    // 文档的第一个子节点应该是声明
    XMLNode* first = doc.FirstChild();
    DOCTEST_REQUIRE(first != nullptr);

    XMLDeclaration* found = first->ToDeclaration();
    DOCTEST_REQUIRE(found != nullptr);

    // Declaration 的 Value() 通常包含声明内容，验证包含 version 关键字
    const char* val = found->Value();
    DOCTEST_CHECK(val != nullptr);
    DOCTEST_CHECK(std::strstr(val, "version") != nullptr);
}

/**
 * 测试用例 13：XMLAttribute - 遍历属性列表
 *
 * 功能说明：
 * - 演示如何使用 FirstAttribute() 遍历元素的属性链表
 * - 验证属性数量和具体键值对
 */
DOCTEST_TEST_CASE("XMLAttribute - 遍历属性")
{
    const char* xml = R"(<elem a="1" b="two" c="true"/>)";
    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xml)));

    XMLElement* e = doc.RootElement();
    DOCTEST_REQUIRE(e != nullptr);

    const XMLAttribute* attr = e->FirstAttribute();
    int count = 0;
    bool foundA = false, foundB = false, foundC = false;
    while (attr)
    {
        count++;
        if (std::strcmp(attr->Name(), "a") == 0)
        {
            foundA = (std::strcmp(attr->Value(), "1") == 0);
        }
        else if (std::strcmp(attr->Name(), "b") == 0)
        {
            foundB = (std::strcmp(attr->Value(), "two") == 0);
        }
        else if (std::strcmp(attr->Name(), "c") == 0)
        {
            foundC = (std::strcmp(attr->Value(), "true") == 0);
        }
        attr = attr->Next();
    }

    DOCTEST_CHECK(count == 3);
    DOCTEST_CHECK(foundA);
    DOCTEST_CHECK(foundB);
    DOCTEST_CHECK(foundC);
}

/**
 * 测试用例 14：DeleteChildren / DeleteChild / InsertFirstChild / InsertAfterChild 操作
 *
 * 功能说明：
 * - 演示删除所有子节点 DeleteChildren()
 * - 演示删除单个子节点 DeleteChild()
 * - 演示在头部插入 InsertFirstChild()
 * - 演示在指定子节点后插入 InsertAfterChild()
 * - 验证子节点顺序和数量
 */
DOCTEST_TEST_CASE("XMLNode - 删除与插入子节点操作")
{
    XMLDocument doc;
    XMLElement* root = doc.NewElement("root");
    doc.InsertEndChild(root);

    // 插入若干子元素
    XMLElement* a = doc.NewElement("a"); a->SetText("A"); root->InsertEndChild(a);
    XMLElement* b = doc.NewElement("b"); b->SetText("B"); root->InsertEndChild(b);
    XMLElement* c = doc.NewElement("c"); c->SetText("C"); root->InsertEndChild(c);

    DOCTEST_CHECK(!root->NoChildren());

    // 删除单个子节点 b
    root->DeleteChild(b);
    // 剩余 a, c
    XMLElement* firstAfterDel = root->FirstChildElement();
    DOCTEST_REQUIRE(firstAfterDel != nullptr);
    DOCTEST_CHECK(std::strcmp(firstAfterDel->Name(), "a") == 0);
    XMLElement* secondAfterDel = firstAfterDel->NextSiblingElement();
    DOCTEST_REQUIRE(secondAfterDel != nullptr);
    DOCTEST_CHECK(std::strcmp(secondAfterDel->Name(), "c") == 0);
    DOCTEST_CHECK(secondAfterDel->NextSiblingElement() == nullptr);

    // 删除所有子节点
    root->DeleteChildren();
    DOCTEST_CHECK(root->NoChildren());

    // 重新插入，使用 InsertEndChild / InsertFirstChild / InsertAfterChild
    XMLElement* n1 = doc.NewElement("n1"); n1->SetText("1");
    XMLElement* n2 = doc.NewElement("n2"); n2->SetText("2");
    root->InsertEndChild(n1);       // n1
    root->InsertFirstChild(n2);     // n2, n1

    XMLElement* first = root->FirstChildElement();
    DOCTEST_REQUIRE(first != nullptr);
    DOCTEST_CHECK(std::strcmp(first->Name(), "n2") == 0);

    // 在 n2 之后插入 n3
    XMLElement* n3 = doc.NewElement("n3"); n3->SetText("3");
    root->InsertAfterChild(n2, n3); // n2, n3, n1

    // 验证顺序
    int idx = 0;
    const char* names[3];
    for (XMLElement* it = root->FirstChildElement(); it; it = it->NextSiblingElement())
    {
        names[idx++] = it->Name();
    }
    DOCTEST_CHECK(idx == 3);
    DOCTEST_CHECK(std::strcmp(names[0], "n2") == 0);
    DOCTEST_CHECK(std::strcmp(names[1], "n3") == 0);
    DOCTEST_CHECK(std::strcmp(names[2], "n1") == 0);
}

/**
 * 测试用例 15：XMLUnknown - 创建和读取未知节点（例如 DOCTYPE）
 *
 * 功能说明：
 * - 演示如何使用 NewUnknown() 创建未知节点（例如 <!DOCTYPE ...>）
 * - 演示如何通过 ToUnknown() 将节点转换回 XMLUnknown 并读取内容
 */
DOCTEST_TEST_CASE("XMLUnknown - 创建与读取未知节点")
{
    XMLDocument doc;
    // 创建未知节点（模拟 <!DOCTYPE sample>）
    XMLUnknown* unk = doc.NewUnknown("!DOCTYPE sample");
    DOCTEST_REQUIRE(unk != nullptr);

    // 将未知节点插入到文档
    doc.InsertEndChild(unk);

    // 文档的第一个节点应为未知节点
    XMLNode* first = doc.FirstChild();
    DOCTEST_REQUIRE(first != nullptr);

    XMLUnknown* found = first->ToUnknown();
    DOCTEST_REQUIRE(found != nullptr);
    DOCTEST_CHECK(std::strcmp(found->Value(), "!DOCTYPE sample") == 0);
}

/**
 * 测试用例 16：DeepCopy / Clear - 文档深拷贝与清理
 *
 * 功能说明：
 * - 演示如何使用 DeepCopy 将一个文档完整复制到另一个文档
 * - 演示 Clear() 清空文档所有内容
 */
DOCTEST_TEST_CASE("XMLDocument - DeepCopy 和 Clear")
{
    XMLDocument src;
    XMLElement* root = src.NewElement("root");
    root->SetAttribute("v", 42);
    XMLElement* child = src.NewElement("child");
    child->SetText("hello");
    root->InsertEndChild(child);
    src.InsertEndChild(root);

    // 目标文档
    XMLDocument dst;
    src.DeepCopy(&dst);

    XMLElement* dstRoot = dst.RootElement();
    DOCTEST_REQUIRE(dstRoot != nullptr);
    DOCTEST_CHECK(std::strcmp(dstRoot->Name(), "root") == 0);
    DOCTEST_CHECK(dstRoot->IntAttribute("v") == 42);

    XMLElement* dstChild = dstRoot->FirstChildElement("child");
    DOCTEST_REQUIRE(dstChild != nullptr);
    DOCTEST_CHECK(std::strcmp(dstChild->GetText(), "hello") == 0);

    // 清理目标文档
    dst.Clear();
    DOCTEST_CHECK(dst.RootElement() == nullptr);
}

/**
 * 测试用例 17：XMLVisitor - 自定义访问者遍历节点
 *
 * 功能说明：
 * - 演示如何继承 XMLVisitor 并重载回调以遍历文档节点
 * - 验证访问者能够按预期访问元素、文本和注释等节点
 */
struct CountingVisitor : public XMLVisitor
{
    int enterCount = 0;
    int textCount = 0;
    int commentCount = 0;

    virtual bool VisitEnter(const XMLElement& /*element*/, const XMLAttribute* /*firstAttribute*/) override
    {
        ++enterCount;
        return true; // 继续遍历子节点
    }

    virtual bool Visit(const XMLText& /*text*/) override
    {
        ++textCount;
        return true;
    }

    virtual bool Visit(const XMLComment& /*comment*/) override
    {
        ++commentCount;
        return true;
    }
};

DOCTEST_TEST_CASE("XMLVisitor - 自定义访问者遍历节点")
{
    const char* xml = R"(<?xml version="1.0"?><root><!--c--><a>t1</a><b>t2</b></root>)";
    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xml)));

    CountingVisitor visitor;
    doc.Accept(&visitor);

    // root, a, b 三个元素进入计数（VisitEnter）
    DOCTEST_CHECK(visitor.enterCount == 3);
    // 两个文本节点（t1, t2）
    DOCTEST_CHECK(visitor.textCount == 2);
    // 一个注释
    DOCTEST_CHECK(visitor.commentCount == 1);
}

/**
 * 测试用例 18：XMLConstHandle - 常量安全链式访问
 *
 * 功能说明：
 * - 演示如何使用 XMLConstHandle 在 const 上下文中链式访问元素
 * - 确保返回的是 const XMLElement*，保持只读语义
 */
DOCTEST_TEST_CASE("XMLConstHandle - 常量安全链式访问")
{
    const char* xml = R"(<root><sub id="9">ok</sub></root>)";
    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xml)));

    // 使用 XMLConstHandle 进行 const 链式访问
    const XMLDocument& cdoc = doc;
    XMLConstHandle h(cdoc);
    const XMLElement* sub = h.FirstChildElement("root").FirstChildElement("sub").ToElement();
    DOCTEST_REQUIRE(sub != nullptr);
    DOCTEST_CHECK(std::strcmp(sub->Attribute("id"), "9") == 0);
    DOCTEST_CHECK(std::strcmp(sub->GetText(), "ok") == 0);

    // 链式访问不存在的节点，安全返回 nullptr
    const XMLElement* notExist = h.FirstChildElement("x").FirstChildElement("y").ToElement();
    DOCTEST_CHECK(notExist == nullptr);
}

/**
 * 测试用例 19：数值类型属性与文本（int64 / float / double）
 *
 * 功能说明：
 * - 演示 SetAttribute 与 Int64Attribute / FloatAttribute / DoubleAttribute 的配合使用
 * - 演示 QueryInt64Attribute / QueryFloatAttribute / QueryDoubleAttribute
 * - 演示 QueryInt64Text 的可用性
 */
DOCTEST_TEST_CASE("XMLElement - int64, float, double 属性与文本")
{
    XMLDocument doc;
    XMLElement* e = doc.NewElement("nums");
    doc.InsertEndChild(e);

    // 设置各种数值属性
    e->SetAttribute("i64", (int64_t)922337203685477LL);
    e->SetAttribute("f", 3.14f);
    e->SetAttribute("d", 2.71828);

    // 读取并断言（使用相应的读取方法）
    DOCTEST_CHECK(e->Int64Attribute("i64") == (int64_t)922337203685477LL);
    DOCTEST_CHECK(std::fabs(e->FloatAttribute("f") - 3.14f) < 1e-6f);
    DOCTEST_CHECK(std::fabs(e->DoubleAttribute("d") - 2.71828) < 1e-9);

    // QueryInt64Attribute 安全查询
    int64_t i64Val = 0;
    XMLError q64err = e->QueryInt64Attribute("i64", &i64Val);
    DOCTEST_CHECK(xml_ok(q64err));
    DOCTEST_CHECK(i64Val == (int64_t)922337203685477LL);

    // QueryDoubleAttribute 安全查询
    double dVal = 0.0;
    XMLError qderr = e->QueryDoubleAttribute("d", &dVal);
    DOCTEST_CHECK(xml_ok(qderr));
    DOCTEST_CHECK(std::fabs(dVal - 2.71828) < 1e-9);

    // QueryFloatAttribute 安全查询
    float fVal = 0.0f;
    XMLError qferr = e->QueryFloatAttribute("f", &fVal);
    DOCTEST_CHECK(xml_ok(qferr));
    DOCTEST_CHECK(std::fabs(fVal - 3.14f) < 1e-6f);

    // 文本数值查询：设置文本并 QueryInt64Text
    e->SetText("12345");
    int64_t iv = 0;
    XMLError qerr = e->QueryInt64Text(&iv);
    DOCTEST_CHECK(xml_ok(qerr));
    DOCTEST_CHECK(iv == 12345);

    // Int64Attribute 默认值
    DOCTEST_CHECK(e->Int64Attribute("notexist") == 0);
    DOCTEST_CHECK(e->Int64Attribute("notexist", 999) == 999);

    // DoubleAttribute 默认值
    DOCTEST_CHECK(e->DoubleAttribute("notexist") == 0.0);
    DOCTEST_CHECK(std::fabs(e->DoubleAttribute("notexist", 1.5) - 1.5) < 1e-9);
}

/**
 * 测试用例 20：实体 & 编码处理与 SaveFile compact 模式
 *
 * 功能说明：
 * - 验证解析包含实体 (&amp; &lt; &gt; &quot; &apos;) 的文本时能正确解码
 * - 验证 Unicode 字符（UTF-8）在序列化后仍被保留
 * - 演示 SaveFile 的 compact 参数（创建文件并使用 RAII 清理）
 */
DOCTEST_TEST_CASE("XML - 实体解码、编码与 SaveFile compact 模式")
{
    const char* xml = R"(<root>
    <e>Tom &amp; Jerry</e>
    <lt>&lt;tag&gt;</lt>
    <quote>&quot;hello&quot;</quote>
    <u>中文测试🙂</u>
</root>)";
    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xml)));

    // 验证 &amp; 实体被解析为 '&'
    XMLElement* e = doc.RootElement()->FirstChildElement("e");
    DOCTEST_REQUIRE(e != nullptr);
    DOCTEST_CHECK(std::strcmp(e->GetText(), "Tom & Jerry") == 0);

    // 验证 &lt; &gt; 实体被解析为 '<' '>'
    XMLElement* lt = doc.RootElement()->FirstChildElement("lt");
    DOCTEST_REQUIRE(lt != nullptr);
    DOCTEST_CHECK(std::strcmp(lt->GetText(), "<tag>") == 0);

    // 验证 &quot; 实体被解析为 '"'
    XMLElement* quote = doc.RootElement()->FirstChildElement("quote");
    DOCTEST_REQUIRE(quote != nullptr);
    DOCTEST_CHECK(std::strcmp(quote->GetText(), "\"hello\"") == 0);

    // 验证 Unicode 文本保留
    XMLElement* u = doc.RootElement()->FirstChildElement("u");
    DOCTEST_REQUIRE(u != nullptr);
    const char* uText = u->GetText();
    DOCTEST_REQUIRE(uText != nullptr);
    DOCTEST_CHECK(std::strcmp(uText, "中文测试🙂") == 0);

    // 保存为普通（非 compact）和 compact 两个文件，主要验证没有错误返回
    const char* f1 = "test_entities_normal.xml";
    const char* f2 = "test_entities_compact.xml";
    TempFileGuard guard1(f1);
    TempFileGuard guard2(f2);

    XMLError s1 = doc.SaveFile(f1, false);
    XMLError s2 = doc.SaveFile(f2, true);
    DOCTEST_CHECK(xml_ok(s1));
    DOCTEST_CHECK(xml_ok(s2));
}

/**
 * 测试用例 21：XMLNode - Parent / GetDocument / PreviousSibling
 *
 * 功能说明：
 * - 演示 Parent() 获取父节点
 * - 演示 GetDocument() 获取节点所属文档
 * - 演示 PreviousSibling() 获取前一个兄弟节点
 */
DOCTEST_TEST_CASE("XMLNode - Parent 和 GetDocument")
{
    const char* xml = R"(<root><child>text</child></root>)";
    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xml)));

    XMLElement* root = doc.RootElement();
    DOCTEST_REQUIRE(root != nullptr);

    XMLElement* child = root->FirstChildElement("child");
    DOCTEST_REQUIRE(child != nullptr);

    // 验证 child 的 Parent 是 root
    XMLNode* parent = child->Parent();
    DOCTEST_REQUIRE(parent != nullptr);
    DOCTEST_CHECK(parent == root);

    // 验证 root 的 Parent 是 document
    XMLNode* rootParent = root->Parent();
    DOCTEST_REQUIRE(rootParent != nullptr);
    DOCTEST_CHECK(rootParent == &doc);

    // 验证 GetDocument 返回正确的文档
    DOCTEST_CHECK(child->GetDocument() == &doc);
    DOCTEST_CHECK(root->GetDocument() == &doc);
}

/**
 * 测试用例 22：XMLNode - DeepClone / ShallowClone
 *
 * 功能说明：
 * - 演示 DeepClone() 深拷贝一个子树到另一个文档
 * - 演示 ShallowClone() 浅拷贝一个节点（不包含子节点）
 */
DOCTEST_TEST_CASE("XMLNode - DeepClone 和 ShallowClone")
{
    XMLDocument src;
    XMLElement* root = src.NewElement("parent");
    root->SetAttribute("x", 1);
    XMLElement* child = src.NewElement("child");
    child->SetText("data");
    root->InsertEndChild(child);
    src.InsertEndChild(root);

    // DeepClone：拷贝整个子树
    XMLDocument dst;
    XMLElement* cloned = root->DeepClone(&dst)->ToElement();
    DOCTEST_REQUIRE(cloned != nullptr);
    DOCTEST_CHECK(std::strcmp(cloned->Name(), "parent") == 0);
    DOCTEST_CHECK(cloned->IntAttribute("x") == 1);

    // DeepClone 应包含子节点
    XMLElement* clonedChild = cloned->FirstChildElement("child");
    DOCTEST_REQUIRE(clonedChild != nullptr);
    DOCTEST_CHECK(std::strcmp(clonedChild->GetText(), "data") == 0);

    // ShallowClone：只拷贝节点本身，不包含子节点
    XMLDocument dst2;
    XMLElement* shallow = root->ShallowClone(&dst2)->ToElement();
    DOCTEST_REQUIRE(shallow != nullptr);
    DOCTEST_CHECK(std::strcmp(shallow->Name(), "parent") == 0);
    DOCTEST_CHECK(shallow->IntAttribute("x") == 1);
    // ShallowClone 不包含子节点
    DOCTEST_CHECK(shallow->FirstChildElement() == nullptr);
}

/**
 * 测试用例 23：XMLDocument - LoadFile 加载不存在的文件
 *
 * 功能说明：
 * - 演示 LoadFile() 加载不存在的文件时返回 XML_ERROR_FILE_NOT_FOUND
 * - 验证错误状态
 */
DOCTEST_TEST_CASE("XMLDocument - LoadFile 不存在的文件")
{
    XMLDocument doc;
    XMLError err = doc.LoadFile("nonexistent_file_12345.xml");
    DOCTEST_CHECK(err == XML_ERROR_FILE_NOT_FOUND);
    DOCTEST_CHECK(doc.Error());
    DOCTEST_CHECK(doc.ErrorID() == XML_ERROR_FILE_NOT_FOUND);
}

/**
 * 测试用例 24：XMLDocument - 空文档和重复解析
 *
 * 功能说明：
 * - 验证新创建的空文档状态
 * - 验证对已有内容的文档再次 Parse 会覆盖内容
 * - 验证解析空字符串的错误处理
 */
DOCTEST_TEST_CASE("XMLDocument - 空文档与重复解析")
{
    // 新创建的文档没有根元素
    XMLDocument doc;
    DOCTEST_CHECK(doc.RootElement() == nullptr);
    DOCTEST_CHECK(!doc.Error());

    // 第一次解析
    XMLError err1 = doc.Parse("<a>1</a>");
    DOCTEST_CHECK(xml_ok(err1));
    DOCTEST_REQUIRE(doc.RootElement() != nullptr);
    DOCTEST_CHECK(std::strcmp(doc.RootElement()->Name(), "a") == 0);

    // 重复解析，内容应被覆盖
    XMLError err2 = doc.Parse("<b>2</b>");
    DOCTEST_CHECK(xml_ok(err2));
    DOCTEST_REQUIRE(doc.RootElement() != nullptr);
    DOCTEST_CHECK(std::strcmp(doc.RootElement()->Name(), "b") == 0);

    // 解析空字符串应返回错误
    XMLError err3 = doc.Parse("");
    DOCTEST_CHECK(!xml_ok(err3));
}

/**
 * 测试用例 25：XMLElement - 类型转换失败
 *
 * 功能说明：
 * - 演示对非数字字符串调用 IntAttribute 时的行为
 * - 演示 QueryIntAttribute 对类型不匹配属性的返回值
 */
DOCTEST_TEST_CASE("XMLElement - 属性类型转换失败")
{
    const char* xml = R"(<elem num="not_a_number" boolval="yes"/>)";
    XMLDocument doc;
    DOCTEST_CHECK(xml_ok(doc.Parse(xml)));

    XMLElement* e = doc.RootElement();
    DOCTEST_REQUIRE(e != nullptr);

    // 对非数字字符串调用 IntAttribute 返回默认值 0
    DOCTEST_CHECK(e->IntAttribute("num") == 0);

    // QueryIntAttribute 对类型不匹配返回 XML_WRONG_ATTRIBUTE_TYPE
    int val = 0;
    XMLError err = e->QueryIntAttribute("num", &val);
    DOCTEST_CHECK(err == XML_WRONG_ATTRIBUTE_TYPE);

    // 对非标准布尔值调用 BoolAttribute（"yes" 不是 "true"/"false"/"1"/"0"）
    bool bval = e->BoolAttribute("boolval");
    // "yes" 不是标准布尔值，BoolAttribute 返回默认值 false
    DOCTEST_CHECK(bval == false);
}

/**
 * 测试用例 26：XML - 只有声明没有元素
 *
 * 功能说明：
 * - 验证解析只有 XML 声明的文档时的行为
 */
DOCTEST_TEST_CASE("XML - 只有声明没有元素")
{
    const char* xml = R"(<?xml version="1.0"?>)";
    XMLDocument doc;
    XMLError err = doc.Parse(xml);
    DOCTEST_CHECK(xml_ok(err));
    DOCTEST_CHECK(doc.RootElement() == nullptr);
}