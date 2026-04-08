#include "doctest.h"
#include "SimpleIni.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>

/**
 * 测试用例 1：从字符串解析 INI（LoadData）并读取基本键值
 *
 * 功能说明：
 * - 演示如何使用 LoadData 从内存字符串解析 INI 内容
 * - 演示 GetValue 在存在/不存在键时返回值或默认值
 */
DOCTEST_TEST_CASE("CSimpleIniA - 从字符串解析并读取键值")
{
    const char* iniStr =
        "; 注释行示例\n"
        "[section]\n"
        "key1 = value1\n"
        "num = 42\n";

    CSimpleIniA ini;
    // 解析内存中的 INI 数据
    SI_Error rc = ini.LoadData(iniStr);
    DOCTEST_CHECK(rc == SI_OK);

    // 读取存在的键
    const char* v1 = ini.GetValue("section", "key1", nullptr);
    DOCTEST_REQUIRE(v1 != nullptr);
    DOCTEST_CHECK(std::strcmp(v1, "value1") == 0);

    // 读取整数字符串（SimpleIni 以字符串为主）
    const char* num = ini.GetValue("section", "num", "0");
    DOCTEST_CHECK(std::strcmp(num, "42") == 0);

    // 读取不存在的键，返回指定的默认值
    const char* none = ini.GetValue("section", "nokey", "def");
    DOCTEST_CHECK(std::strcmp(none, "def") == 0);
}

/**
 * 测试用例 2：设置值、保存到文件并重新加载验证（LoadFile/SaveFile）
 *
 * 功能说明：
 * - 演示 SetValue 添加或修改键值
 * - 演示 SaveFile 将 INI 写入文件
 * - 演示 LoadFile 重新加载并验证持久化结果
 */
DOCTEST_TEST_CASE("CSimpleIniA - SetValue / SaveFile / LoadFile roundtrip")
{
    const char* filename = "test_simpleini.ini";

    // 使用 RAII 确保即使测试中途失败也能清理临时文件
    struct FileGuard
    {
        const char* f;
        explicit FileGuard(const char* fname) : f(fname) {}
        ~FileGuard() { std::remove(f); }
    } guard(filename);

    // 第一步：创建并保存
    {
        CSimpleIniA ini;
        ini.SetUnicode(); // 使用 UTF-8 支持（对 CSimpleIniA 可调用以保持一致）
        ini.SetValue("server", "host", "localhost");
        ini.SetValue("server", "port", "8080");
        ini.SetValue("user", "name", "alice");

        SI_Error s = ini.SaveFile(filename);
        DOCTEST_CHECK(s == SI_OK);
    }

    // 第二步：从文件加载并验证
    {
        CSimpleIniA ini;
        SI_Error l = ini.LoadFile(filename);
        DOCTEST_CHECK(l == SI_OK);

        const char* host = ini.GetValue("server", "host", nullptr);
        DOCTEST_REQUIRE(host != nullptr);
        DOCTEST_CHECK(std::strcmp(host, "localhost") == 0);

        const char* port = ini.GetValue("server", "port", nullptr);
        DOCTEST_REQUIRE(port != nullptr);
        DOCTEST_CHECK(std::strcmp(port, "8080") == 0);

        const char* name = ini.GetValue("user", "name", nullptr);
        DOCTEST_REQUIRE(name != nullptr);
        DOCTEST_CHECK(std::strcmp(name, "alice") == 0);
    }
}

/**
 * 测试用例 3：删除键与节（Delete）
 *
 * 功能说明：
 * - 演示如何使用 Delete 中的 API 删除键或整个节（如果存在）
 * - 验证删除后 GetValue 返回默认值或 nullptr
 * - 验证删除不存在的键/节返回 false
 */
DOCTEST_TEST_CASE("CSimpleIniA - 删除键与节")
{
    CSimpleIniA ini;
    ini.SetValue("s", "k1", "v1");
    ini.SetValue("s", "k2", "v2");

    // 删除单个键
    bool d1 = ini.Delete("s", "k1");
    DOCTEST_CHECK(d1 == true);

    const char* k1 = ini.GetValue("s", "k1", nullptr);
    DOCTEST_CHECK(k1 == nullptr);

    // 删除不存在的键应返回 false
    bool d1_again = ini.Delete("s", "k1");
    DOCTEST_CHECK(d1_again == false);

    // 删除整个节
    bool d2 = ini.Delete("s", nullptr);
    DOCTEST_CHECK(d2 == true);

    // 此时节不存在，获取任一键均返回默认
    const char* k2 = ini.GetValue("s", "k2", "nod");
    DOCTEST_CHECK(std::strcmp(k2, "nod") == 0);

    // 删除不存在的节应返回 false
    bool d3 = ini.Delete("nonexist", nullptr);
    DOCTEST_CHECK(d3 == false);
}

/**
 * 测试用例 4：注释、空节与默认值行为
 *
 * 功能说明：
 * - 演示注释行（以 ';' 或 '#' 开头）被忽略
 * - 演示空节（无键）仍可存在
 * - 演示 GetValue 的默认值参数在键缺失时生效
 */
DOCTEST_TEST_CASE("CSimpleIniA - 注释与默认值处理")
{
    const char* iniStr =
        "# 全行注释\n"
        "[empty]\n"
        "; 另一注释\n"
        "[data]\n"
        "k = v\n";

    CSimpleIniA ini;
    SI_Error rc = ini.LoadData(iniStr);
    DOCTEST_CHECK(rc == SI_OK);

    // 空节存在，但没有键
    const char* emptyVal = ini.GetValue("empty", "any", "dft");
    DOCTEST_CHECK(std::strcmp(emptyVal, "dft") == 0);

    // 注释行不会作为键
    const char* k = ini.GetValue("data", "k", nullptr);
    DOCTEST_REQUIRE(k != nullptr);
    DOCTEST_CHECK(std::strcmp(k, "v") == 0);
}

/**
 * 测试用例 5：多行值与保留换行（SetValue 含 \\n + Save/Load roundtrip）
 *
 * 功能说明：
 * - 演示在开启 SetMultiLine(true) 后，将包含换行符的值通过 SetValue 写入
 * - 验证 Save 到字符串后重新 LoadData 仍能保留换行
 * - 注意：未开启 SetMultiLine 时，含换行符的值经序列化/反序列化后会丢失
 */
DOCTEST_TEST_CASE("CSimpleIniA - 多行值示例（SetValue 含换行符 + roundtrip）")
{
    CSimpleIniA ini;
    ini.SetMultiLine(true);
    const char* multi = "line1\nline2\nline3";
    // 将多行文本作为值（以 '\n' 字符存在字符串中）
    ini.SetValue("m", "multiline", multi);

    // 内存中直接读取应一致
    const char* got = ini.GetValue("m", "multiline", nullptr);
    DOCTEST_REQUIRE(got != nullptr);
    DOCTEST_CHECK(std::strcmp(got, multi) == 0);

    // 保存到字符串后重新解析，验证换行符被保留
    std::string saved;
    SI_Error sret = ini.Save(saved, false);
    DOCTEST_CHECK(sret == SI_OK);

    CSimpleIniA ini2;
    ini2.SetMultiLine(true);
    SI_Error lret = ini2.LoadData(saved);
    DOCTEST_CHECK(lret == SI_OK);

    const char* got2 = ini2.GetValue("m", "multiline", nullptr);
    DOCTEST_REQUIRE(got2 != nullptr);
    DOCTEST_CHECK(std::strcmp(got2, multi) == 0);
}

/**
 * 测试用例 6：Unicode / UTF-8 支持（以 UTF-8 串测试）
 *
 * 功能说明：
 * - 演示在值中使用 UTF-8（如中文或表情），并验证读写保留内容
 * - 验证 SaveFile/LoadFile 在写入时不破坏 UTF-8 字节
 */
DOCTEST_TEST_CASE("CSimpleIniA - Unicode (UTF-8) 支持")
{
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.SetValue("u", "name", "中文测试🙂");
    const char* got = ini.GetValue("u", "name", nullptr);
    DOCTEST_REQUIRE(got != nullptr);
    DOCTEST_CHECK(std::strcmp(got, "中文测试🙂") == 0);

    // 验证 SaveFile/LoadFile 不破坏 UTF-8 字节
    const char* filename = "test_utf8.ini";
    struct FileGuard
    {
        const char* f;
        explicit FileGuard(const char* fname) : f(fname) {}
        ~FileGuard() { std::remove(f); }
    } guard(filename);

    SI_Error sret = ini.SaveFile(filename);
    DOCTEST_CHECK(sret == SI_OK);

    CSimpleIniA ini2;
    ini2.SetUnicode();
    SI_Error lret = ini2.LoadFile(filename);
    DOCTEST_CHECK(lret == SI_OK);

    const char* got2 = ini2.GetValue("u", "name", nullptr);
    DOCTEST_REQUIRE(got2 != nullptr);
    DOCTEST_CHECK(std::strcmp(got2, "中文测试🙂") == 0);
}

/**
 * 测试用例 7：遍历节与键（GetAllSections / GetAllKeys）
 *
 * 功能说明：
 * - 演示获取节名列表（GetAllSections）与键名列表（GetAllKeys）
 * - 说明：SimpleIni 提供多种迭代器接口，下面以常见方式展示基础遍历思想
 */
DOCTEST_TEST_CASE("CSimpleIniA - 遍历节与键（GetAllSections / GetAllKeys）")
{
    CSimpleIniA ini;
    ini.SetValue("A", "k1", "v1");
    ini.SetValue("A", "k2", "v2");
    ini.SetValue("B", "kx", "vx");

    // 获取所有节名
    CSimpleIniA::TNamesDepend sections;
    ini.GetAllSections(sections);
    DOCTEST_CHECK(sections.size() == 2);

    // 获取节 "A" 和 "B" 的键集合
    CSimpleIniA::TNamesDepend keysA;
    ini.GetAllKeys("A", keysA);
    DOCTEST_CHECK(keysA.size() == 2);

    CSimpleIniA::TNamesDepend keysB;
    ini.GetAllKeys("B", keysB);
    DOCTEST_CHECK(keysB.size() == 1);

    // 对不存在的节获取键应返回 false
    CSimpleIniA::TNamesDepend keysNone;
    bool found = ini.GetAllKeys("NoSection", keysNone);
    DOCTEST_CHECK(found == false);
    DOCTEST_CHECK(keysNone.size() == 0);
}

/**
 * 测试用例 8：覆盖/重复键的行为（最后写入覆盖）
 *
 * 功能说明：
 * - 演示在 SetMultiKey(false)（默认）时，同一节中多次 SetValue 相同键，最后一次生效
 * - 演示 SetValue 返回 SI_INSERTED（首次插入）和 SI_UPDATED（更新）
 */
DOCTEST_TEST_CASE("CSimpleIniA - 覆盖/重复键行为（SetMultiKey=false）")
{
    CSimpleIniA ini;
    ini.SetMultiKey(false); // 显式关闭 multi-key（默认值）

    SI_Error r1 = ini.SetValue("s", "k", "v1");
    DOCTEST_CHECK(r1 == SI_INSERTED);

    SI_Error r2 = ini.SetValue("s", "k", "v2"); // 覆盖之前的值
    DOCTEST_CHECK(r2 == SI_UPDATED);

    const char* got = ini.GetValue("s", "k", nullptr);
    DOCTEST_REQUIRE(got != nullptr);
    DOCTEST_CHECK(std::strcmp(got, "v2") == 0);
}

/**
 * 测试用例 9：空节名与全局键（无节键）处理
 *
 * 功能说明：
 * - 演示将键设置到全局（无节）区域（使用空字符串作为节名）
 * - 验证读取与删除行为
 */
DOCTEST_TEST_CASE("CSimpleIniA - 全局键（无节）处理")
{
    CSimpleIniA ini;
    ini.SetValue("", "global", "gval");

    const char* g = ini.GetValue("", "global", nullptr);
    DOCTEST_REQUIRE(g != nullptr);
    DOCTEST_CHECK(std::strcmp(g, "gval") == 0);

    // 删除全局键
    ini.Delete("", "global");
    const char* after = ini.GetValue("", "global", "dft");
    DOCTEST_CHECK(std::strcmp(after, "dft") == 0);
}

/**
 * 测试用例 10：错误与边界情况（LoadFile 失败返回值）
 *
 * 功能说明：
 * - 验证 LoadFile 在文件不存在或不可读时返回错误
 */
DOCTEST_TEST_CASE("CSimpleIniA - LoadFile 错误返回")
{
    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile("this_file_does_not_exist.ini");
    DOCTEST_CHECK(rc != SI_OK);
}

/**
 * 测试用例 11：multi-key 场景（SetMultiKey / GetAllValues / DeleteValue）
 *
 * 功能说明：
 * - 演示开启多值支持 SetMultiKey(true)
 * - 演示使用 SetValue 插入相同 key 多次
 * - 演示 GetAllValues 返回全部值，GetValue 返回第一个值
 * - 演示 DeleteValue(section,key,value) 仅删除匹配的值
 */
DOCTEST_TEST_CASE("CSimpleIniA - multi-key 与 GetAllValues / DeleteValue")
{
    CSimpleIniA ini;
    ini.SetMultiKey(true);

    // 插入两条同名键
    ini.SetValue("s", "k", "v1");
    ini.SetValue("s", "k", "v2");

    // GetAllValues 应返回两条值（按插入/加载顺序）
    CSimpleIniA::TNamesDepend values;
    bool got = ini.GetAllValues("s", "k", values);
    DOCTEST_CHECK(got);
    DOCTEST_CHECK(values.size() == 2);

    // 第一条应为 v1，GetValue 返回第一条（文档行为）
    auto it = values.begin();
    DOCTEST_CHECK(std::strcmp(it->pItem, "v1") == 0);
    DOCTEST_CHECK(std::strcmp(ini.GetValue("s", "k", nullptr), "v1") == 0);

    // 删除指定值 v1，仅保留 v2
    bool del = ini.DeleteValue("s", "k", "v1", true);
    DOCTEST_CHECK(del);

    // 再次获取所有值应只剩下 v2
    values.clear();
    ini.GetAllValues("s", "k", values);
    DOCTEST_CHECK(values.size() == 1);
    DOCTEST_CHECK(std::strcmp(values.begin()->pItem, "v2") == 0);
}

/**
 * 测试用例 12：数值与布尔 API（SetLongValue/GetLongValue / SetDoubleValue/GetDoubleValue / SetBoolValue/GetBoolValue）
 *
 * 功能说明：
 * - 验证长整型、浮点型、布尔型的读写接口
 * - 验证 SetLongValue 的十六进制输出（a_bUseHex = true）能够被 GetLongValue 正确解析
 * - 验证 SetBoolValue 写出的字符串格式
 * - 验证 GetBoolValue 对 yes/no/on/off/1/0 的解析
 */
DOCTEST_TEST_CASE("CSimpleIniA - Get/Set Long/Double/Bool")
{
    CSimpleIniA ini;

    // long 值：首次插入返回 SI_INSERTED
    SI_Error r1 = ini.SetLongValue("n", "i", 12345);
    DOCTEST_CHECK(r1 == SI_INSERTED);
    long li = ini.GetLongValue("n", "i", 0);
    DOCTEST_CHECK(li == 12345);

    // 再次设置同一键返回 SI_UPDATED
    SI_Error r2 = ini.SetLongValue("n", "i", 99999);
    DOCTEST_CHECK(r2 == SI_UPDATED);
    DOCTEST_CHECK(ini.GetLongValue("n", "i", 0) == 99999);

    // hex 写入，验证字符串格式包含 "0x" 或 "2A" 及数值正确
    ini.SetLongValue("n", "hex", 0x2A, NULL, true);
    long hx = ini.GetLongValue("n", "hex", 0);
    DOCTEST_CHECK(hx == 0x2A);
    const char* hexStr = ini.GetValue("n", "hex", nullptr);
    DOCTEST_REQUIRE(hexStr != nullptr);
    // 十六进制格式使用 "0x%lx"，0x2A 值为 42，输出为 "0x2a"（小写）
    DOCTEST_CHECK(std::strcmp(hexStr, "0x2a") == 0);

    // double 值
    ini.SetDoubleValue("n", "d", 3.14159);
    double dv = ini.GetDoubleValue("n", "d", 0.0);
    DOCTEST_CHECK(std::fabs(dv - 3.14159) < 1e-8);

    // 布尔值：SetBoolValue 写入 "true"/"false"
    ini.SetBoolValue("f", "b1", true);
    ini.SetBoolValue("f", "b2", false);

    DOCTEST_CHECK(ini.GetBoolValue("f", "b1", false) == true);
    DOCTEST_CHECK(ini.GetBoolValue("f", "b2", true) == false);

    // 验证 SetBoolValue 写出的字符串格式
    const char* b1str = ini.GetValue("f", "b1", nullptr);
    DOCTEST_REQUIRE(b1str != nullptr);
    DOCTEST_CHECK(std::strcmp(b1str, "true") == 0);

    const char* b2str = ini.GetValue("f", "b2", nullptr);
    DOCTEST_REQUIRE(b2str != nullptr);
    DOCTEST_CHECK(std::strcmp(b2str, "false") == 0);

    // 测试字符串解析规则：yes/no/on/off/1/0
    ini.SetValue("f", "s_yes", "yes");
    ini.SetValue("f", "s_no", "no");
    ini.SetValue("f", "s_on", "on");
    ini.SetValue("f", "s_off", "off");
    ini.SetValue("f", "s_1", "1");
    ini.SetValue("f", "s_0", "0");

    DOCTEST_CHECK(ini.GetBoolValue("f", "s_yes", false) == true);
    DOCTEST_CHECK(ini.GetBoolValue("f", "s_no", true) == false);
    DOCTEST_CHECK(ini.GetBoolValue("f", "s_on", false) == true);
    DOCTEST_CHECK(ini.GetBoolValue("f", "s_off", true) == false);
    DOCTEST_CHECK(ini.GetBoolValue("f", "s_1", false) == true);
    DOCTEST_CHECK(ini.GetBoolValue("f", "s_0", true) == false);

    // 无法识别的字符串应返回默认值
    ini.SetValue("f", "s_unknown", "maybe");
    DOCTEST_CHECK(ini.GetBoolValue("f", "s_unknown", true) == true);
    DOCTEST_CHECK(ini.GetBoolValue("f", "s_unknown", false) == false);
}

/**
 * 测试用例 13：真正的 multi-line 标签解析与序列化（<<<ENDTAG 语法）
 *
 * 功能说明：
 * - 演示 LoadData 解析以 <<<TAG 开头并以 TAG 结尾的多行值
 * - 演示 Save(std::string&) 将多行值以 <<<END_OF_TEXT 形式序列化
 */
DOCTEST_TEST_CASE("CSimpleIniA - multi-line tag 解析与 Save 到字符串")
{
    // 构造一个包含 multi-line tag 的 INI 文本（文件注释 + 节 + 多行值）
    const char* iniText =
        ";file comment line1\n"
        "[ml]\n"
        "big = <<<END\n"
        "lineA\n"
        "lineB\n"
        "lineC\n"
        "END\n";

    CSimpleIniA ini;
    // 启用 multi-line 支持以解析 <<<TAG 语法
    ini.SetMultiLine(true);
    SI_Error rc = ini.LoadData(iniText);
    DOCTEST_CHECK(rc == SI_OK);

    // 读取值，值中的换行应被规范化为单个 '\n'
    const char* big = ini.GetValue("ml", "big", nullptr);
    DOCTEST_REQUIRE(big != nullptr);
    // 精确匹配：多行值应为 "lineA\nlineB\nlineC"
    DOCTEST_CHECK(std::strcmp(big, "lineA\nlineB\nlineC") == 0);

    // 将内容保存到 std::string 并检查序列化是否使用了 multi-line 输出格式
    std::string out;
    SI_Error sret = ini.Save(out, false);
    DOCTEST_CHECK(sret == SI_OK);
    // 序列化时库使用固定的 END_OF_TEXT 标识符来写多行数据
    DOCTEST_CHECK(out.find("<<<END_OF_TEXT") != std::string::npos);
    DOCTEST_CHECK(out.find("END_OF_TEXT") != std::string::npos);
}

/**
 * 测试用例 14：Save 到 std::string（StringWriter）和注释保留测试
 *
 * 功能说明：
 * - 演示 Save(std::string&) 将 INI 写入内存字符串
 * - 验证加载的注释（file/section/key）在 Save 时被保留
 * - 验证 Save 后重新 LoadData 仍能完整还原数据
 */
DOCTEST_TEST_CASE("CSimpleIniA - Save to string & 注释保留 & roundtrip")
{
    const char* iniText =
        "; FILE_COMMENT\n"
        "; still file comment\n"
        "[sec]\n"
        "; SECTION_COMMENT\n"
        "; another section comment\n"
        "; KEY_COMMENT\n"
        "key = value\n";

    CSimpleIniA ini;
    SI_Error rc = ini.LoadData(iniText);
    DOCTEST_CHECK(rc == SI_OK);

    // 保存到字符串
    std::string out;
    SI_Error sret = ini.Save(out, false);
    DOCTEST_CHECK(sret == SI_OK);

    // 验证保存的字符串包含文件/节/键的注释片段
    DOCTEST_CHECK(out.find("FILE_COMMENT") != std::string::npos);
    DOCTEST_CHECK(out.find("SECTION_COMMENT") != std::string::npos);
    // key 注释可能紧贴在 key 之前或之后，但应存在于输出中
    DOCTEST_CHECK(out.find("KEY_COMMENT") != std::string::npos);

    // 从保存的字符串重新加载，验证数据完整性
    CSimpleIniA ini2;
    SI_Error lret = ini2.LoadData(out);
    DOCTEST_CHECK(lret == SI_OK);

    const char* val = ini2.GetValue("sec", "key", nullptr);
    DOCTEST_REQUIRE(val != nullptr);
    DOCTEST_CHECK(std::strcmp(val, "value") == 0);
}

/**
 * 测试用例 15：SectionExists / KeyExists / GetSectionSize / GetSection（返回 TKeyVal 并遍历）
 *
 * 功能说明：
 * - 验证节存在性、键存在性、节大小以及通过 GetSection 获取节内键值映射
 * - 验证对不存在的节，GetSectionSize 返回 -1，GetSection 返回 nullptr
 */
DOCTEST_TEST_CASE("CSimpleIniA - SectionExists / KeyExists / GetSectionSize / GetSection")
{
    CSimpleIniA ini;
    ini.SetValue("S", "k1", "v1");
    ini.SetValue("S", "k2", "v2");
    ini.SetValue("S", "k2", "v2b"); // 未开启 multi-key，k2 被覆盖为 v2b

    DOCTEST_CHECK(ini.SectionExists("S") == true);
    DOCTEST_CHECK(ini.SectionExists("NoSection") == false);
    DOCTEST_CHECK(ini.KeyExists("S", "k1") == true);
    DOCTEST_CHECK(ini.KeyExists("S", "nok") == false);

    int sz = ini.GetSectionSize("S");
    // 未开启 multi-key 时大小应为精确 2（k1, k2）
    DOCTEST_CHECK(sz == 2);

    // 不存在的节应返回 -1
    DOCTEST_CHECK(ini.GetSectionSize("NoSection") == -1);

    // 获取节的原始映射并遍历
    const CSimpleIniA::TKeyVal* sec = ini.GetSection("S");
    DOCTEST_REQUIRE(sec != nullptr);
    DOCTEST_CHECK(sec->size() == 2);

    // 简单遍历检查键名与值（不作严格顺序假设）
    int entries = 0;
    for (auto it = sec->begin(); it != sec->end(); ++it) {
        const CSimpleIniA::Entry &entry = it->first;
        const char* val = it->second;
        DOCTEST_CHECK(entry.pItem != nullptr);
        DOCTEST_CHECK(val != nullptr);
        ++entries;
    }
    DOCTEST_CHECK(entries == 2);

    // 不存在的节应返回 nullptr
    const CSimpleIniA::TKeyVal* nosec = ini.GetSection("NoSection");
    DOCTEST_CHECK(nosec == nullptr);
}

/**
 * 测试用例 16：空字符串值
 *
 * 功能说明：
 * - 验证 SetValue 设置空字符串后 GetValue 返回空字符串（而非 nullptr）
 * - 验证空字符串值经过 Save/Load roundtrip 后仍为空字符串
 */
DOCTEST_TEST_CASE("CSimpleIniA - 空字符串值")
{
    CSimpleIniA ini;
    ini.SetValue("s", "empty", "");

    // GetValue 应返回空字符串而非 nullptr
    const char* v = ini.GetValue("s", "empty", nullptr);
    DOCTEST_REQUIRE(v != nullptr);
    DOCTEST_CHECK(std::strcmp(v, "") == 0);

    // Save/Load roundtrip 验证
    std::string saved;
    SI_Error sret = ini.Save(saved, false);
    DOCTEST_CHECK(sret == SI_OK);

    CSimpleIniA ini2;
    SI_Error lret = ini2.LoadData(saved);
    DOCTEST_CHECK(lret == SI_OK);

    const char* v2 = ini2.GetValue("s", "empty", nullptr);
    DOCTEST_REQUIRE(v2 != nullptr);
    DOCTEST_CHECK(std::strcmp(v2, "") == 0);
}

/**
 * 测试用例 17：multi-key 场景下 SetValue a_bForceReplace 和 Delete 整节
 *
 * 功能说明：
 * - 演示在 multi-key 模式下，SetValue 的 a_bForceReplace=true 替换所有同名键
 * - 演示 multi-key 模式下 Delete(section, nullptr) 删除整个节
 */
DOCTEST_TEST_CASE("CSimpleIniA - multi-key 下 ForceReplace 与 Delete 整节")
{
    CSimpleIniA ini;
    ini.SetMultiKey(true);

    // 插入多条同名键
    ini.SetValue("s", "k", "v1");
    ini.SetValue("s", "k", "v2");
    ini.SetValue("s", "k", "v3");

    // 验证有 3 条值
    CSimpleIniA::TNamesDepend values;
    ini.GetAllValues("s", "k", values);
    DOCTEST_CHECK(values.size() == 3);

    // a_bForceReplace=true 替换所有同名键为单个值
    ini.SetValue("s", "k", "replaced", nullptr, true);
    values.clear();
    ini.GetAllValues("s", "k", values);
    DOCTEST_CHECK(values.size() == 1);
    DOCTEST_CHECK(std::strcmp(values.begin()->pItem, "replaced") == 0);

    // 再添加多条用于测试 Delete 整节
    ini.SetValue("s", "k", "extra1");
    ini.SetValue("s", "other", "val");

    // 删除整个节
    bool del = ini.Delete("s", nullptr);
    DOCTEST_CHECK(del == true);

    DOCTEST_CHECK(ini.SectionExists("s") == false);
    DOCTEST_CHECK(ini.GetValue("s", "k", "dft") != nullptr);
    DOCTEST_CHECK(std::strcmp(ini.GetValue("s", "k", "dft"), "dft") == 0);
}

/**
 * 测试用例 18：GetLongValue / GetDoubleValue 的 a_pHasMultiple 参数
 *
 * 功能说明：
 * - 验证在 multi-key 模式下，GetLongValue/GetDoubleValue 的 a_pHasMultiple 输出参数
 *   在有多个同名键时返回 true
 */
DOCTEST_TEST_CASE("CSimpleIniA - GetLongValue/GetDoubleValue a_pHasMultiple")
{
    CSimpleIniA ini;
    ini.SetMultiKey(true);

    // 单个值时 a_pHasMultiple 应为 false
    ini.SetLongValue("n", "i", 42);
    bool hasMultiple = false;
    long lv = ini.GetLongValue("n", "i", 0, &hasMultiple);
    DOCTEST_CHECK(lv == 42);
    DOCTEST_CHECK(hasMultiple == false);

    // 再添加一个同名键
    ini.SetValue("n", "i", "100");
    hasMultiple = false;
    long lv2 = ini.GetLongValue("n", "i", 0, &hasMultiple);
    DOCTEST_CHECK(lv2 == 42); // 返回第一个值
    DOCTEST_CHECK(hasMultiple == true);

    // Double 的 a_pHasMultiple
    ini.SetDoubleValue("n", "d", 1.5);
    hasMultiple = false;
    double dv = ini.GetDoubleValue("n", "d", 0.0, &hasMultiple);
    DOCTEST_CHECK(std::fabs(dv - 1.5) < 1e-8);
    DOCTEST_CHECK(hasMultiple == false);
}

/**
 * 测试用例 19：畸形 INI 数据的容错解析
 *
 * 功能说明：
 * - 验证 LoadData 对畸形数据不崩溃，尽可能解析有效内容
 * - 测试只有 key 没有 = 的情况、空行、重复节名等边界场景
 */
DOCTEST_TEST_CASE("CSimpleIniA - 畸形数据容错")
{
    // 包含空行、只有 key 没有 =、重复节名
    const char* iniStr =
        "\n"
        "[sec]\n"
        "key_only_no_eq\n"
        "normal = value\n"
        "\n"
        "[sec]\n"          // 重复节名
        "another = v2\n";

    CSimpleIniA ini;
    SI_Error rc = ini.LoadData(iniStr);
    // 不崩溃即算通过，返回值可能是 SI_OK（SimpleIni 容错）
    DOCTEST_CHECK(rc == SI_OK);

    // 正常的键值应可读取
    const char* normal = ini.GetValue("sec", "normal", nullptr);
    DOCTEST_REQUIRE(normal != nullptr);
    DOCTEST_CHECK(std::strcmp(normal, "value") == 0);
}