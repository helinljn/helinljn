#include "doctest.h"
#include "SimpleIni.h"
#include "core/common.h"
#include <climits>
#include <filesystem>

namespace fs = std::filesystem;

DOCTEST_TEST_SUITE("Ini")
{
    DOCTEST_TEST_CASE("BaseUseage")
    {
        const char* input =
            "; This is values comment\n"
            "  [values]  \n"
            "enabled =   TRUE   \n"
            "disabled=False\n"
            "long_value = 123456\n"
            "  double_value = 123.456\n"
            "svalue0 = hello world  ; this is not a comment\n"
            "  svalue1   = \"HELLO WORLD\"\n"
            "  svalue2   =   Hello World\n  "
            "svalue3 = \"  hello world  \"\n";

        // 总是设置为utf-8编码
        CSimpleIni ini(true);

        DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

        DOCTEST_CHECK(ini.GetSectionSize("values") == 8);
        DOCTEST_CHECK(ini.SectionExists("values"));
        DOCTEST_CHECK(ini.KeyExists("values", "svalue0"));
        DOCTEST_CHECK(ini.KeyExists("values", "svalue1"));
        DOCTEST_CHECK(ini.KeyExists("values", "svalue2"));
        DOCTEST_CHECK(ini.KeyExists("values", "svalue3"));

        DOCTEST_CHECK(ini.GetBoolValue("values", "enabled", false));
        DOCTEST_CHECK(!ini.GetBoolValue("values", "disabled", true));
        DOCTEST_CHECK(ini.GetLongValue("values", "long_value", 0) == 123456);
        DOCTEST_CHECK(ini.GetDoubleValue("values", "double_value", 0.0) == 123.456);
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("values", "svalue0"), "hello world  ; this is not a comment") == 0);
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("values", "svalue1"), "\"HELLO WORLD\"") == 0);
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("values", "svalue2"), "Hello World") == 0);
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("values", "svalue3"), "\"  hello world  \"") == 0);

        std::string save_str;
        DOCTEST_CHECK(ini.Save(save_str) == SI_OK);

        // 因为双平台的结束符不一样，所以统一移除'\r'
        save_str.erase(std::remove(save_str.begin(), save_str.end(), '\r'), save_str.end());

        const char* expect_str =
            "; This is values comment\n"
            "\n"
            "\n"
            "[values]\n"
            "enabled = TRUE\n"
            "disabled = False\n"
            "long_value = 123456\n"
            "double_value = 123.456\n"
            "svalue0 = hello world  ; this is not a comment\n"
            "svalue1 = \"HELLO WORLD\"\n"
            "svalue2 = Hello World\n"
            "svalue3 = \"  hello world  \"\n";
        DOCTEST_CHECK(save_str == expect_str);

        DOCTEST_CHECK(ini.SaveFile("test.ini") == SI_OK);

        ini.Reset();
        DOCTEST_CHECK(ini.LoadFile("test.ini") == SI_OK);

        save_str.clear();
        DOCTEST_CHECK(ini.Save(save_str) == SI_OK);

        // 因为双平台的结束符不一样，所以统一移除'\r'
        save_str.erase(std::remove(save_str.begin(), save_str.end(), '\r'), save_str.end());
        DOCTEST_CHECK(save_str == expect_str);

        DOCTEST_CHECK(fs::remove("test.ini"));

        ini.Reset();
        DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

        // 增删改查操作
        DOCTEST_CHECK(ini.SetValue("values", "svalue4", "hello world") == SI_INSERTED);
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("values", "svalue4"), "hello world") == 0);

        save_str.clear();
        DOCTEST_CHECK(ini.Save(save_str) == SI_OK);

        // 因为双平台的结束符不一样，所以统一移除'\r'
        save_str.erase(std::remove(save_str.begin(), save_str.end(), '\r'), save_str.end());
        DOCTEST_CHECK(core::concat(expect_str, "svalue4 = hello world\n") == save_str);

        DOCTEST_CHECK(ini.SetValue("values", "svalue4", "hello world 4") == SI_UPDATED);
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("values", "svalue4"), "hello world 4") == 0);

        save_str.clear();
        DOCTEST_CHECK(ini.Save(save_str) == SI_OK);

        // 因为双平台的结束符不一样，所以统一移除'\r'
        save_str.erase(std::remove(save_str.begin(), save_str.end(), '\r'), save_str.end());
        DOCTEST_CHECK(core::concat(expect_str, "svalue4 = hello world 4\n") == save_str);

        DOCTEST_CHECK(ini.Delete("values", "svalue4"));

        save_str.clear();
        DOCTEST_CHECK(ini.Save(save_str) == SI_OK);

        // 因为双平台的结束符不一样，所以统一移除'\r'
        save_str.erase(std::remove(save_str.begin(), save_str.end(), '\r'), save_str.end());
        DOCTEST_CHECK(save_str == expect_str);
    }

    DOCTEST_TEST_CASE("TestBoolean")
    {
        // 测试 true 值
        {
            const char* input =
                "[bools]\n"
                "true1 = true\n"
                "true2 = t\n"
                "true3 = yes\n"
                "true4 = y\n"
                "true5 = 1\n"
                "true6 = on\n";

            // 总是设置为utf-8编码
            CSimpleIni ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.GetBoolValue("bools", "true1", false));
            DOCTEST_CHECK(ini.GetBoolValue("bools", "true2", false));
            DOCTEST_CHECK(ini.GetBoolValue("bools", "true3", false));
            DOCTEST_CHECK(ini.GetBoolValue("bools", "true4", false));
            DOCTEST_CHECK(ini.GetBoolValue("bools", "true5", false));
            DOCTEST_CHECK(ini.GetBoolValue("bools", "true6", false));
        }

        // 测试 false 值
        {
            const char* input =
                "[bools]\n"
                "false1 = false\n"
                "false2 = f\n"
                "false3 = no\n"
                "false4 = n\n"
                "false5 = 0\n"
                "false6 = off\n";

            // 总是设置为utf-8编码
            CSimpleIni ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(!ini.GetBoolValue("bools", "false1", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "false2", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "false3", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "false4", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "false5", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "false6", true));
        }

        // 测试忽略大小写
        {
            const char* input =
                "[bools]\n"
                "upper = TRUE\n"
                "mixed = YeS\n"
                "lower = false\n"
                "caps = NO\n";

            // 总是设置为utf-8编码
            CSimpleIni ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.GetBoolValue("bools", "upper", false));
            DOCTEST_CHECK(ini.GetBoolValue("bools", "mixed", false));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "lower", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "caps", true));
        }

        // 测试未识别的值返回默认值
        {
            const char* input =
                "[bools]\n"
                "invalid1 = maybe\n"
                "invalid2 = 2\n"
                "invalid3 = \n"
                "invalid4 = enabled\n";

            // 总是设置为utf-8编码
            CSimpleIni ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.GetBoolValue("bools", "invalid1", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "invalid1", false));

            DOCTEST_CHECK(ini.GetBoolValue("bools", "invalid2", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "invalid2", false));

            DOCTEST_CHECK(ini.GetBoolValue("bools", "invalid3", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "invalid3", false));

            DOCTEST_CHECK(ini.GetBoolValue("bools", "invalid4", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "invalid4", false));
        }

        // 测试 Key 不存在时返回默认值
        {
            const char* input = "[bools]\n";

            // 总是设置为utf-8编码
            CSimpleIni ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.GetBoolValue("bools", "missing", true));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "missing", false));

            DOCTEST_CHECK(ini.GetBoolValue("missing_section", "key", true));
            DOCTEST_CHECK(!ini.GetBoolValue("missing_section", "key", false));
        }

        // 测试重复设置时更新值
        {
            // 总是设置为utf-8编码
            CSimpleIni ini(true);

            DOCTEST_CHECK(ini.SetBoolValue("bools", "toggle", true) == SI_INSERTED);
            DOCTEST_CHECK(ini.GetBoolValue("bools", "toggle", false));

            DOCTEST_CHECK(ini.SetBoolValue("bools", "toggle", false) == SI_UPDATED);
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "toggle", true));
        }

        // 测试 SetBoolValue 的输出格式
        {
            // 总是设置为utf-8编码
            CSimpleIni ini(true);

            DOCTEST_CHECK(ini.SetBoolValue("bools", "enabled", true) == SI_INSERTED);
            DOCTEST_CHECK(ini.SetBoolValue("bools", "disabled", false) == SI_INSERTED);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            DOCTEST_CHECK(core::contains(output, "enabled = true"));
            DOCTEST_CHECK(core::contains(output, "disabled = false"));
        }

        // 测试 GetBoolValue 与空白字符相关的问题
        {
            const char* input =
                "[bools]\n"
                "padded =   true   \n"
                "tabs =\tfalse\t\n";

            // 总是设置为utf-8编码
            CSimpleIni ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.GetBoolValue("bools", "padded", false));
            DOCTEST_CHECK(!ini.GetBoolValue("bools", "tabs", true));
        }
    }

    DOCTEST_TEST_CASE("BugFix")
    {
        // 测试空节的处理
        {
            CSimpleIni ini(true);
            ini.SetValue("foo", "skey", "sval");
            ini.SetValue("", "rkey", "rval");
            ini.SetValue("bar", "skey", "sval");

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            std::string expected =
                "rkey = rval\n"
                "\n"
                "\n"
                "[foo]\n"
                "skey = sval\n"
                "\n"
                "\n"
                "[bar]\n"
                "skey = sval\n";

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            DOCTEST_CHECK(output == expected);
        }

        // 测试多行值忽略尾部空格（无空格）
        {
            const char* input =
                "; multiline values\n"
                "key = <<<EOS\n"
                "This is a\n"
                "multiline value\n"
                "and it ends.\n"
                "EOS\n"
                "\n"
                "[section]\n";

            bool multiline = true;
            CSimpleIniA ini(true, false, multiline);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            std::string expected =
                "; multiline values\n"
                "\n"
                "\n"
                "key = <<<END_OF_TEXT\n"
                "This is a\n"
                "multiline value\n"
                "and it ends.\n"
                "END_OF_TEXT\n"
                "\n"
                "\n"
                "[section]\n";

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            DOCTEST_CHECK(output == expected);
        }

        // 测试多行值忽略尾部空格（一个空格）
        {
            const char* input =
                "; multiline values\n"
                "key = <<<EOS\n"
                "This is a\n"
                "multiline value\n"
                "and it ends.\n"
                "EOS \n"
                "\n"
                "[section]\n";

            bool multiline = true;
            CSimpleIniA ini(true, false, multiline);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            std::string expected =
                "; multiline values\n"
                "\n"
                "\n"
                "key = <<<END_OF_TEXT\n"
                "This is a\n"
                "multiline value\n"
                "and it ends.\n"
                "END_OF_TEXT\n"
                "\n"
                "\n"
                "[section]\n";

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            DOCTEST_CHECK(output == expected);
        }

        // 测试多行值忽略尾部空格（两个空格）
        {
            const char* input =
                "; multiline values\n"
                "key = <<<EOS\n"
                "This is a\n"
                "multiline value\n"
                "and it ends.\n"
                "EOS  \n"
                "\n"
                "[section]\n";

            bool multiline = true;
            CSimpleIniA ini(true, false, multiline);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            std::string expected =
                "; multiline values\n"
                "\n"
                "\n"
                "key = <<<END_OF_TEXT\n"
                "This is a\n"
                "multiline value\n"
                "and it ends.\n"
                "END_OF_TEXT\n"
                "\n"
                "\n"
                "[section]\n";

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            DOCTEST_CHECK(output == expected);
        }
    }

    DOCTEST_TEST_CASE("CaseSensitivity")
    {
        // 测试不区分大小写的节名
        {
            const char* input =
                "[Section]\n"
                "key = value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.SectionExists("Section"));
            DOCTEST_CHECK(ini.SectionExists("SECTION"));
            DOCTEST_CHECK(ini.SectionExists("section"));
            DOCTEST_CHECK(ini.SectionExists("SeCTioN"));

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("Section", "key"), "value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("SECTION", "key"), "value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value") == 0);
        }

        // 测试不区分大小写的键名
        {
            const char* input =
                "[section]\n"
                "Key = value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.KeyExists("section", "Key"));
            DOCTEST_CHECK(ini.KeyExists("section", "KEY"));
            DOCTEST_CHECK(ini.KeyExists("section", "key"));
            DOCTEST_CHECK(ini.KeyExists("section", "kEy"));

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "Key"), "value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "KEY"), "value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value") == 0);
        }

        // 测试不区分大小写的 SetValue 更新现有值
        {
            const char* input =
                "[Section]\n"
                "Key = value1\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.SetValue("SECTION", "KEY", "value2") == SI_UPDATED);

            DOCTEST_CHECK(ini.GetSectionSize("section") == 1);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value2") == 0);
        }

        // 测试区分大小写的节名
        {
            const char* input =
                "[Section]\n"
                "key = value1\n"
                "\n"
                "[SECTION]\n"
                "key = value2\n"
                "\n"
                "[section]\n"
                "key = value3\n";

            CSimpleIniCaseA ini;
            ini.SetUnicode();

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.SectionExists("Section"));
            DOCTEST_CHECK(ini.SectionExists("SECTION"));
            DOCTEST_CHECK(ini.SectionExists("section"));
            DOCTEST_CHECK(!ini.SectionExists("SeCTioN"));

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("Section", "key"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("SECTION", "key"), "value2") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value3") == 0);

            CSimpleIniCaseA::TNamesDepend sections;
            ini.GetAllSections(sections);
            DOCTEST_CHECK(sections.size() == 3);
        }

        // 测试区分大小写的键名
        {
            const char* input =
                "[section]\n"
                "Key = value1\n"
                "KEY = value2\n"
                "key = value3\n";

            CSimpleIniCaseA ini;
            ini.SetUnicode();

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.KeyExists("section", "Key"));
            DOCTEST_CHECK(ini.KeyExists("section", "KEY"));
            DOCTEST_CHECK(ini.KeyExists("section", "key"));
            DOCTEST_CHECK(!ini.KeyExists("section", "kEy"));

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "Key"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "KEY"), "value2") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value3") == 0);

            DOCTEST_CHECK(ini.GetSectionSize("section") == 3);
        }

        // 测试区分大小写的 SetValue 创建新条目
        {
            const char* input =
                "[Section]\n"
                "Key = value1\n";

            CSimpleIniCaseA ini;
            ini.SetUnicode();

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.SetValue("SECTION", "KEY", "value2") == SI_INSERTED);

            DOCTEST_CHECK(ini.SetValue("Section", "Key", "value3") == SI_UPDATED);

            CSimpleIniCaseA::TNamesDepend sections;
            ini.GetAllSections(sections);
            DOCTEST_CHECK(sections.size() == 2);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("Section", "Key"), "value3") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("SECTION", "KEY"), "value2") == 0);
        }

        // 测试不区分大小写的删除
        {
            const char* input =
                "[Section]\n"
                "Key = value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.Delete("SECTION", "KEY"));

            DOCTEST_CHECK(!ini.KeyExists("section", "key"));
        }

        // 测试区分大小写的删除
        {
            const char* input =
                "[Section]\n"
                "Key = value1\n"
                "KEY = value2\n";

            CSimpleIniCaseA ini;
            ini.SetUnicode();

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.Delete("Section", "Key"));

            DOCTEST_CHECK(!ini.KeyExists("Section", "Key"));
            DOCTEST_CHECK(ini.KeyExists("Section", "KEY"));
        }

        // 测试不区分大小写的 GetAllKeys
        {
            const char* input =
                "[section]\n"
                "key1 = value1\n"
                "KEY1 = value2\n"
                "key2 = value3\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            CSimpleIniA::TNamesDepend keys;
            ini.GetAllKeys("section", keys);

            DOCTEST_CHECK(keys.size() == 2);
        }

        // 测试区分大小写的 GetAllKeys
        {
            const char* input =
                "[section]\n"
                "key1 = value1\n"
                "KEY1 = value2\n"
                "key2 = value3\n";

            CSimpleIniCaseA ini;
            ini.SetUnicode();

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            CSimpleIniCaseA::TNamesDepend keys;
            ini.GetAllKeys("section", keys);

            DOCTEST_CHECK(keys.size() == 3);
        }

        // 测试值总是区分大小写
        {
            const char* input =
                "[section]\n"
                "key = Value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            DOCTEST_CHECK(core::stringcmp(value, "Value") == 0);
            DOCTEST_CHECK(core::stringcmp(value, "value") != 0);
            DOCTEST_CHECK(core::stringcmp(value, "VALUE") != 0);
        }

        // 测试不区分大小写模式下的 Unicode 字符（只有 ASCII 不区分大小写）
        {
            CSimpleIniA ini(true);

            const char lower[] = u8"testé";
            const char upper[] = u8"TESTÉ";

            ini.SetValue("section", lower, "value1");
            ini.SetValue("section", upper, "value2");

            const char* val1 = ini.GetValue("section", lower);
            const char* val2 = ini.GetValue("section", upper);

            DOCTEST_CHECK(val1 != nullptr);
            DOCTEST_CHECK(val2 != nullptr);
        }

        // 测试不区分大小写的往返保留原始大小写
        {
            const char* input =
                "[MixedCase]\n"
                "MixedKey = MixedValue\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());

            DOCTEST_CHECK(core::contains(output, "[MixedCase]"));
            DOCTEST_CHECK(core::contains(output, "MixedKey"));
            DOCTEST_CHECK(core::contains(output, "MixedValue"));
        }

        // 测试区分大小写的节在不同情况下是独立的
        {
            CSimpleIniCaseA ini;
            ini.SetUnicode();

            ini.SetValue("section", "key", "value1");
            ini.SetValue("Section", "key", "value2");
            ini.SetValue("SECTION", "key", "value3");

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("Section", "key"), "value2") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("SECTION", "key"), "value3") == 0);

            DOCTEST_CHECK(ini.Delete("Section", nullptr));

            DOCTEST_CHECK(ini.SectionExists("section"));
            DOCTEST_CHECK(!ini.SectionExists("Section"));
            DOCTEST_CHECK(ini.SectionExists("SECTION"));
        }
    }

    DOCTEST_TEST_CASE("EdgeCases")
    {
        // 测试节名中的特殊字符
        {
            std::string input =
                "[section-with-dashes]\n"
                "key = value1\n"
                "\n"
                "[section_with_underscores]\n"
                "key = value2\n"
                "\n"
                "[section.with.dots]\n"
                "key = value3\n"
                "\n"
                "[section:with:colons]\n"
                "key = value4\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section-with-dashes", "key"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section_with_underscores", "key"), "value2") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section.with.dots", "key"), "value3") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section:with:colons", "key"), "value4") == 0);
        }

        // 测试键名中的特殊字符
        {
            std::string input =
                "[section]\n"
                "key-with-dashes = value1\n"
                "key_with_underscores = value2\n"
                "key.with.dots = value3\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key-with-dashes"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key_with_underscores"), "value2") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key.with.dots"), "value3") == 0);
        }

        // 测试值中的等号
        {
            std::string input =
                "[section]\n"
                "key1 = value=with=equals\n"
                "key2 = a=b\n"
                "key3 = ===\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key1"), "value=with=equals") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key2"), "a=b") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key3"), "===") == 0);
        }

        // 测试值中的分号（注释字符）
        {
            std::string input =
                "[section]\n"
                "key = value ; this is not a comment\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value ; this is not a comment") == 0);
        }

        // 测试值中的井号（注释字符）
        {
            std::string input =
                "[section]\n"
                "key = value # this is not a comment\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value # this is not a comment") == 0);
        }

        // 测试值中的方括号
        {
            std::string input =
                "[section]\n"
                "key1 = [value]\n"
                "key2 = ]value[\n"
                "key3 = [[nested]]\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key1"), "[value]") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key2"), "]value[") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key3"), "[[nested]]") == 0);
        }

        // 测试非常长的节名
        {
            std::string longName(1000, 'a');
            std::string input = "[" + longName + "]\nkey=value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.SectionExists(longName.c_str()));
            DOCTEST_CHECK(core::stringcmp(ini.GetValue(longName.c_str(), "key"), "value") == 0);
        }

        // 测试非常长的键名
        {
            std::string longKey(1000, 'b');
            std::string input = "[section]\n" + longKey + "=value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", longKey.c_str()), "value") == 0);
        }

        // 测试非常长的值
        {
            std::string longValue(10000, 'c');
            CSimpleIniA ini(true);
            ini.SetValue("section", "key", longValue.c_str());

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), longValue) == 0);
        }

        // 测试节名中的前导空白
        {
            std::string input =
                "[  section  ]\n"
                "key = value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.SectionExists("section"));
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value") == 0);
        }

        // 测试键名中的前导和尾部空白
        {
            std::string input =
                "[section]\n"
                "  key  = value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.KeyExists("section", "key"));
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value") == 0);
        }

        // 测试值中的前导和尾部空白
        {
            std::string input =
                "[section]\n"
                "key =   value   \n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value") == 0);
        }

        // 测试制表符作为空白
        {
            std::string input =
                "[\tsection\t]\n"
                "\tkey\t=\tvalue\t\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(ini.SectionExists("section"));
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value") == 0);
        }

        // 测试空行和多个空行
        {
            std::string input =
                "\n\n\n"
                "[section1]\n"
                "\n\n"
                "key1 = value1\n"
                "\n\n\n"
                "[section2]\n"
                "\n"
                "key2 = value2\n"
                "\n\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section1", "key1"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section2", "key2"), "value2") == 0);
        }

        // 测试不同的换行格式
        {
            std::string input =
                "[section1]\r\n"
                "key1 = value1\n"
                "[section2]\r"
                "key2 = value2\r\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section1", "key1"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section2", "key2"), "value2") == 0);
        }

        // 测试格式错误的节（没有右括号）
        {
            std::string input =
                "[section\n"
                "key = value\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);
        }

        // 测试一行中有多个等号
        {
            std::string input =
                "[section]\n"
                "key = value = more = data\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value = more = data") == 0);
        }

        // 测试空值与没有等号
        {
            CSimpleIniA ini(true);
            ini.SetAllowKeyOnly(true);

            std::string input =
                "[section]\n"
                "key1 = \n"
                "key2\n";

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* val1 = ini.GetValue("section", "key1");
            const char* val2 = ini.GetValue("section", "key2");

            DOCTEST_CHECK(ini.KeyExists("section", "key1"));
            DOCTEST_CHECK(ini.KeyExists("section", "key2"));

            DOCTEST_CHECK(val1 != nullptr);
            DOCTEST_CHECK(core::stringcmp(val1, "") == 0);

            DOCTEST_CHECK(val2 != nullptr);
            DOCTEST_CHECK(core::stringcmp(val2, "") == 0);
        }

        // 测试 Unicode 节名/键名/值名
        {
            const char tesuto[] = u8"テスト";
            const char kensa[] = u8"検査";
            const char value[] = u8"値";

            CSimpleIniA ini(true);
            ini.SetValue(tesuto, kensa, value);

            const char* result = ini.GetValue(tesuto, kensa);
            DOCTEST_CHECK(core::stringcmp(result, value) == 0);
        }

        // 测试多个节
        {
            CSimpleIniA ini(true);
            for (int i = 0; i < 1000; i++)
            {
                ini.SetValue(core::concat("section", std::to_string(i)).c_str(), "key", "value");
            }

            CSimpleIniA::TNamesDepend sections;
            ini.GetAllSections(sections);
            DOCTEST_CHECK(sections.size() == 1000);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section0", "key"), "value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section500", "key"), "value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section999", "key"), "value") == 0);
        }

        // 测试一个节中的多个键
        {
            CSimpleIniA ini(true);
            for (int i = 0; i < 1000; i++)
            {
                ini.SetValue("section", core::concat("key", std::to_string(i)).c_str(), "value");
            }

            DOCTEST_CHECK(ini.GetSectionSize("section") == 1000);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key0"), "value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key500"), "value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key999"), "value") == 0);
        }

        // 测试带注释的 SetValue
        {
            CSimpleIniA ini(true);
            DOCTEST_CHECK(ini.SetValue("section", "key", "value", "; This is a comment") == SI_INSERTED);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "value") == 0);

            std::string output;
            ini.Save(output);

            DOCTEST_CHECK(core::contains(output, "; This is a comment"));
        }

        // 测试没有节的键（应该放到空节中）
        {
            std::string input =
                "key1 = value1\n"
                "\n"
                "[section]\n"
                "key2 = value2\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("", "key1"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key2"), "value2") == 0);
        }

        // 测试连续的同名节（应该合并或覆盖）
        {
            std::string input =
                "[section]\n"
                "key1 = value1\n"
                "\n"
                "[section]\n"
                "key2 = value2\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key1"), "value1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key2"), "value2") == 0);
            DOCTEST_CHECK(ini.GetSectionSize("section") == 2);
        }

        // 测试只有空白的值
        {
            std::string input =
                "[section]\n"
                "key1 =     \n"
                "key2 = \t\t\t\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* val1 = ini.GetValue("section", "key1");
            const char* val2 = ini.GetValue("section", "key2");
            DOCTEST_CHECK(val1 != nullptr);
            DOCTEST_CHECK(val2 != nullptr);
            DOCTEST_CHECK(core::stringcmp(val1, "") == 0);
            DOCTEST_CHECK(core::stringcmp(val2, "") == 0);
        }
    }

    DOCTEST_TEST_CASE("Multiline")
    {
        // 测试基本的多行值
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "Line 1\n"
                "Line 2\n"
                "Line 3\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            DOCTEST_CHECK(value != nullptr);

            std::string expected = "Line 1\nLine 2\nLine 3";
            DOCTEST_CHECK(core::stringcmp(value, expected) == 0);
        }

        // 测试不同的结束标签
        {
            std::string input =
                "[section]\n"
                "key1 = <<<EOF\n"
                "content1\n"
                "EOF\n"
                "key2 = <<<MARKER\n"
                "content2\n"
                "MARKER\n"
                "key3 = <<<123\n"
                "content3\n"
                "123\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key1"), "content1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key2"), "content2") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key3"), "content3") == 0);
        }

        // 测试空内容的多行值
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            DOCTEST_CHECK(value != nullptr);
            DOCTEST_CHECK(core::stringcmp(value, "") == 0);
        }

        // 测试带有前导/尾部空行的多行值
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "\n"
                "content\n"
                "\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key"), "\ncontent\n") == 0);
        }

        // 测试带有特殊字符的多行值
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "Line with = equals\n"
                "Line with # hash\n"
                "Line with ; semicolon\n"
                "Line with [brackets]\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            std::string expected =
                "Line with = equals\n"
                "Line with # hash\n"
                "Line with ; semicolon\n"
                "Line with [brackets]";
            DOCTEST_CHECK(core::stringcmp(value, expected) == 0);
        }

        // 测试内容中包含结束标签的多行值
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "This line has END in it\n"
                "  END with leading spaces\n"
                "END with trailing text after\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            std::string expected =
                "This line has END in it\n"
                "  END with leading spaces\n"
                "END with trailing text after";
            DOCTEST_CHECK(core::stringcmp(value, expected) == 0);
        }

        // 测试 SetValue 创建包含换行符的多行值
        {
            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            std::string multilineValue = "Line 1\nLine 2\nLine 3";
            DOCTEST_CHECK(ini.SetValue("section", "key", multilineValue.c_str()) == SI_INSERTED);

            const char* value = ini.GetValue("section", "key");
            DOCTEST_CHECK(core::stringcmp(value, multilineValue) == 0);
        }

        // 测试多行值的往返
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "Line 1\n"
                "Line 2\n"
                "Line 3\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            // 重新加载
            CSimpleIniA ini2(true);
            ini2.SetMultiLine(true);
            DOCTEST_CHECK(ini2.LoadData(output) == SI_OK);

            const char* value = ini2.GetValue("section", "key");
            std::string expected = "Line 1\nLine 2\nLine 3";
            DOCTEST_CHECK(core::stringcmp(value, expected) == 0);
        }

        // 测试多个多行值
        {
            std::string input =
                "[section]\n"
                "key1 = <<<END1\n"
                "Content 1\n"
                "END1\n"
                "key2 = regular value\n"
                "key3 = <<<END3\n"
                "Content 3\n"
                "END3\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key1"), "Content 1") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key2"), "regular value") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section", "key3"), "Content 3") == 0);
        }

        // 测试未启用 SetMultiLine 的情况
        {
            CSimpleIniA ini2(true);
            ini2.SetMultiLine(false);

            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "Line 1\n"
                "END\n";

            DOCTEST_CHECK(ini2.LoadData(input) == SI_OK);

            // 应该被视为常规值
            const char* value = ini2.GetValue("section", "key");
            DOCTEST_CHECK(value != nullptr);
            // 值应该包含 "<<<"
            DOCTEST_CHECK(std::string(value).find("<<<") != std::string::npos);
        }

        // 测试带有制表符和空格的多行值
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "\tTabbed line\n"
                "    Spaced line\n"
                "Mixed\t \twhitespace\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            std::string expected = "\tTabbed line\n    Spaced line\nMixed\t \twhitespace";
            DOCTEST_CHECK(core::stringcmp(value, expected) == 0);
        }

        // 测试带有 Unicode 内容的多行值
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                u8"日本語\n"
                u8"한국어\n"
                u8"中文\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            std::string expected = u8"日本語\n한국어\n中文";
            DOCTEST_CHECK(core::stringcmp(value, expected) == 0);
        }

        // 测试非常长的多行值
        {
            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            std::string longContent;
            for (int i = 0; i < 1000; i++) {
                longContent += "Line " + std::to_string(i) + "\n";
            }
            // 移除尾部换行符
            if (!longContent.empty() && longContent.back() == '\n') {
                longContent.pop_back();
            }

            ini.SetValue("section", "key", longContent.c_str());

            const char* value = ini.GetValue("section", "key");
            DOCTEST_CHECK(core::stringcmp(value, longContent) == 0);
        }

        // 测试带有空格的多行标签（应该被修剪）
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "content\n"
                "END  \n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            DOCTEST_CHECK(core::stringcmp(value, "content") == 0);
        }

        // 测试空节中的多行值
        {
            std::string input =
                "key = <<<END\n"
                "content\n"
                "END\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("", "key");
            DOCTEST_CHECK(core::stringcmp(value, "content") == 0);
        }

        // 测试带有 Windows 换行符的多行值
        {
            std::string input =
                "[section]\r\n"
                "key = <<<END\r\n"
                "Line 1\r\n"
                "Line 2\r\n"
                "END\r\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* value = ini.GetValue("section", "key");
            // 内部表示使用 \n
            std::string expected = "Line 1\nLine 2";
            DOCTEST_CHECK(core::stringcmp(value, expected) == 0);
        }

        // 测试多行值后跟另一个节
        {
            std::string input =
                "[section1]\n"
                "key = <<<END\n"
                "multiline\n"
                "content\n"
                "END\n"
                "\n"
                "[section2]\n"
                "key2 = value2\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section1", "key"), "multiline\ncontent") == 0);
            DOCTEST_CHECK(core::stringcmp(ini.GetValue("section2", "key2"), "value2") == 0);
        }

        // 测试格式错误的多行值（缺少结束标签）
        {
            std::string input =
                "[section]\n"
                "key = <<<END\n"
                "content\n"
                "[section2]\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            // 应该优雅处理 - 要么错误要么视为常规值
            ini.LoadData(input);
        }

        // 测试带有空标签的多行值
        {
            std::string input =
                "[section]\n"
                "key = <<<\n"
                "content\n"
                "\n";

            CSimpleIniA ini(true);
            ini.SetMultiLine(true);

            // 空标签的行为取决于实现
            // 只是验证它不会崩溃
            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);
        }
    }

    DOCTEST_TEST_CASE("Numeric")
    {
        // 测试 GetLongValue 与有效整数
        {
            std::string input =
                "[numbers]\n"
                "positive = 42\n"
                "zero = 0\n"
                "negative = -123\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            long result = ini.GetLongValue("numbers", "positive", 0);
            DOCTEST_CHECK(result == 42);

            result = ini.GetLongValue("numbers", "zero", -1);
            DOCTEST_CHECK(result == 0);

            result = ini.GetLongValue("numbers", "negative", 0);
            DOCTEST_CHECK(result == -123);
        }

        // 测试 GetLongValue 与十六进制值
        {
            std::string input =
                "[numbers]\n"
                "hex1 = 0xFF\n"
                "hex2 = 0x10\n"
                "hex3 = 0x12345678\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            long result = ini.GetLongValue("numbers", "hex1", 0);
            DOCTEST_CHECK(result == 0xFF);

            result = ini.GetLongValue("numbers", "hex2", 0);
            DOCTEST_CHECK(result == 0x10);

            result = ini.GetLongValue("numbers", "hex3", 0);
            DOCTEST_CHECK(result == 0x12345678);
        }

        // 测试 GetLongValue 与无效值（应该返回默认值）
        {
            std::string input =
                "[numbers]\n"
                "text = hello\n"
                "empty = \n"
                "partial = 123abc\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            long result = ini.GetLongValue("numbers", "text", 999);
            DOCTEST_CHECK(result == 999);

            // 空字符串返回默认值（SimpleIni 行为，不是原始 strtol）
            result = ini.GetLongValue("numbers", "empty", 999);
            DOCTEST_CHECK(result == 999);

            // "123abc" 返回默认值 - SimpleIni 验证整个字符串
            result = ini.GetLongValue("numbers", "partial", 999);
            DOCTEST_CHECK(result == 999);
        }

        // 测试 GetLongValue 与不存在的键
        {
            std::string input = "[numbers]\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            long result = ini.GetLongValue("numbers", "missing", 777);
            DOCTEST_CHECK(result == 777);

            result = ini.GetLongValue("missing_section", "key", 888);
            DOCTEST_CHECK(result == 888);
        }

        // 测试 SetLongValue
        {
            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.SetLongValue("numbers", "value1", 12345) == SI_INSERTED);

            long result = ini.GetLongValue("numbers", "value1", 0);
            DOCTEST_CHECK(result == 12345);

            // 更新现有值
            DOCTEST_CHECK(ini.SetLongValue("numbers", "value1", 67890) == SI_UPDATED);

            result = ini.GetLongValue("numbers", "value1", 0);
            DOCTEST_CHECK(result == 67890);
        }

        // 测试 SetLongValue 与十六进制格式
        {
            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.SetLongValue("numbers", "hexval", 255, nullptr, true) == SI_INSERTED);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            // 应该以十六进制写入
            DOCTEST_CHECK(output.find("0xff") != std::string::npos);

            // 应该正确读回
            long result = ini.GetLongValue("numbers", "hexval", 0);
            DOCTEST_CHECK(result == 255);
        }

        // 测试 SetLongValue 与负值
        {
            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.SetLongValue("numbers", "negative", -9999) == SI_INSERTED);

            long result = ini.GetLongValue("numbers", "negative", 0);
            DOCTEST_CHECK(result == -9999);
        }

        // 测试 GetDoubleValue 与有效双精度数
        {
            std::string input =
                "[floats]\n"
                "pi = 3.14159\n"
                "negative = -2.5\n"
                "integer = 42.0\n"
                "scientific = 1.23e-4\n"
                "zero = 0.0\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            double result = ini.GetDoubleValue("floats", "pi", 0.0);
            DOCTEST_CHECK(std::abs(result - 3.14159) < 0.00001);

            result = ini.GetDoubleValue("floats", "negative", 0.0);
            DOCTEST_CHECK(std::abs(result - (-2.5)) < 0.00001);

            result = ini.GetDoubleValue("floats", "integer", 0.0);
            DOCTEST_CHECK(std::abs(result - 42.0) < 0.00001);

            result = ini.GetDoubleValue("floats", "scientific", 0.0);
            DOCTEST_CHECK(std::abs(result - 1.23e-4) < 0.000001);

            result = ini.GetDoubleValue("floats", "zero", 1.0);
            DOCTEST_CHECK(std::abs(result - 0.0) < 0.00001);
        }

        // 测试 GetDoubleValue 与无效值
        {
            std::string input =
                "[floats]\n"
                "text = not_a_number\n"
                "empty = \n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            double result = ini.GetDoubleValue("floats", "text", 99.9);
            DOCTEST_CHECK(std::abs(result - 99.9) < 0.00001);

            result = ini.GetDoubleValue("floats", "empty", 88.8);
            DOCTEST_CHECK(std::abs(result - 88.8) < 0.00001);
        }

        // 测试 SetDoubleValue
        {
            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.SetDoubleValue("floats", "value1", 3.14159) == SI_INSERTED);

            double result = ini.GetDoubleValue("floats", "value1", 0.0);
            DOCTEST_CHECK(std::abs(result - 3.14159) < 0.00001);

            // 更新现有值
            DOCTEST_CHECK(ini.SetDoubleValue("floats", "value1", 2.71828) == SI_UPDATED);

            result = ini.GetDoubleValue("floats", "value1", 0.0);
            DOCTEST_CHECK(std::abs(result - 2.71828) < 0.00001);
        }

        // 测试 SetDoubleValue 与负值和科学记数法
        {
            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.SetDoubleValue("floats", "negative", -123.456) == SI_INSERTED);

            double result = ini.GetDoubleValue("floats", "negative", 0.0);
            DOCTEST_CHECK(std::abs(result - (-123.456)) < 0.0001);

            DOCTEST_CHECK(ini.SetDoubleValue("floats", "tiny", 0.000001) == SI_INSERTED);

            result = ini.GetDoubleValue("floats", "tiny", 0.0);
            DOCTEST_CHECK(std::abs(result - 0.000001) < 0.0000001);
        }

        // 测试多键与数值
        {
            CSimpleIniA ini(true);
            ini.SetMultiKey(true);

            std::string input =
                "[numbers]\n"
                "value = 10\n"
                "value = 20\n"
                "value = 30\n";

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            bool hasMultiple = false;
            long result = ini.GetLongValue("numbers", "value", 0, &hasMultiple);
            DOCTEST_CHECK(result == 10);
            DOCTEST_CHECK(hasMultiple);

            // 获取所有值
            CSimpleIniA::TNamesDepend values;
            ini.GetAllValues("numbers", "value", values);
            DOCTEST_CHECK(values.size() == 3);
        }

        // 测试在多键模式下使用 SetLongValue 强制替换
        {
            CSimpleIniA ini(true);
            ini.SetMultiKey(true);

            std::string input =
                "[numbers]\n"
                "value = 10\n"
                "value = 20\n";

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            // 替换所有值
            DOCTEST_CHECK(ini.SetLongValue("numbers", "value", 999, nullptr, false, true) == SI_UPDATED);

            // 现在应该只有一个值
            bool hasMultiple = false;
            long result = ini.GetLongValue("numbers", "value", 0, &hasMultiple);
            DOCTEST_CHECK(result == 999);
            DOCTEST_CHECK(!hasMultiple);
        }

        // 测试极值
        {
            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.SetLongValue("numbers", "max", LONG_MAX) == SI_INSERTED);

            DOCTEST_CHECK(ini.SetLongValue("numbers", "min", LONG_MIN) == SI_INSERTED);

            long result = ini.GetLongValue("numbers", "max", 0);
            DOCTEST_CHECK(result == LONG_MAX);

            result = ini.GetLongValue("numbers", "min", 0);
            DOCTEST_CHECK(result == LONG_MIN);
        }

        // 测试带有空白的数值
        {
            std::string input =
                "[numbers]\n"
                "padded =   42   \n"
                "tabs =\t123\t\n";

            CSimpleIniA ini(true);

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            long result = ini.GetLongValue("numbers", "padded", 0);
            DOCTEST_CHECK(result == 42);

            result = ini.GetLongValue("numbers", "tabs", 0);
            DOCTEST_CHECK(result == 123);
        }

        // 测试数值往返
        {
            CSimpleIniA ini(true);

            ini.SetLongValue("test", "long1", 12345);
            ini.SetLongValue("test", "long2", -67890);
            ini.SetDoubleValue("test", "double1", 3.14159);
            ini.SetDoubleValue("test", "double2", -2.71828);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            // 重新加载
            CSimpleIniA ini2(true);
            DOCTEST_CHECK(ini2.LoadData(output) == SI_OK);

            DOCTEST_CHECK(ini2.GetLongValue("test", "long1", 0) == 12345);
            DOCTEST_CHECK(ini2.GetLongValue("test", "long2", 0) == -67890);
            DOCTEST_CHECK(std::abs(ini2.GetDoubleValue("test", "double1", 0.0) - 3.14159) < 0.00001);
            DOCTEST_CHECK(std::abs(ini2.GetDoubleValue("test", "double2", 0.0) - (-2.71828)) < 0.00001);
        }
    }

    DOCTEST_TEST_CASE("Quotes")
    {
        // 测试空值
        {
            CSimpleIniA ini(true);
            ini.SetQuotes(true);

            std::string input =
                "[section]\n"
                "key1 = \"\"\n"
                "key2 = \n";

            // 不需要为空数据保留引号
            std::string expect =
                "[section]\n"
                "key1 = \n"
                "key2 = \n";

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* result = ini.GetValue("section", "key1");
            DOCTEST_CHECK(core::stringcmp(result, "") == 0);

            result = ini.GetValue("section", "key2");
            DOCTEST_CHECK(core::stringcmp(result, "") == 0);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            DOCTEST_CHECK(core::stringcmp(expect, output) == 0);
        }

        // 测试禁用引号时的空值
        {
            CSimpleIniA ini(true);
            ini.SetQuotes(false);

            std::string input =
                "[section]\n"
                "key1 = \"\"\n"
                "key2 = \n";

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* result = ini.GetValue("section", "key1");
            DOCTEST_CHECK(core::stringcmp(result, "\"\"") == 0);

            result = ini.GetValue("section", "key2");
            DOCTEST_CHECK(core::stringcmp(result, "") == 0);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            DOCTEST_CHECK(core::stringcmp(input, output) == 0);
        }

        // 测试一般情况
        {
            CSimpleIniA ini(true);
            ini.SetQuotes(true);

            std::string input =
                "[section]\n"
                "key1 = foo\n"
                "key2 = \"foo\"\n"
                "key3 =  foo \n"
                "key4 = \" foo \"\n"
                "key5 = \"foo\n"
                "key6 = foo\"\n"
                "key7 =  foo \" foo \n"
                "key8 =  \" foo \" foo \" \n";

            std::string expect =
                "[section]\n"
                "key1 = foo\n"
                "key2 = foo\n"
                "key3 = foo\n"
                "key4 = \" foo \"\n"
                "key5 = \"foo\n"
                "key6 = foo\"\n"
                "key7 = foo \" foo\n"
                "key8 = \" foo \" foo \"\n";

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            const char* result = ini.GetValue("section", "key1");
            DOCTEST_CHECK(core::stringcmp(result, "foo") == 0);

            result = ini.GetValue("section", "key2");
            DOCTEST_CHECK(core::stringcmp(result, "foo") == 0);

            result = ini.GetValue("section", "key3");
            DOCTEST_CHECK(core::stringcmp(result, "foo") == 0);

            result = ini.GetValue("section", "key4");
            DOCTEST_CHECK(core::stringcmp(result, " foo ") == 0);

            result = ini.GetValue("section", "key5");
            DOCTEST_CHECK(core::stringcmp(result, "\"foo") == 0);

            result = ini.GetValue("section", "key6");
            DOCTEST_CHECK(core::stringcmp(result, "foo\"") == 0);

            result = ini.GetValue("section", "key7");
            DOCTEST_CHECK(core::stringcmp(result, "foo \" foo") == 0);

            result = ini.GetValue("section", "key8");
            DOCTEST_CHECK(core::stringcmp(result, " foo \" foo ") == 0);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            DOCTEST_CHECK(core::stringcmp(expect, output) == 0);
        }

        // 测试禁用引号时的一般情况
        {
            CSimpleIniA ini(true);
            ini.SetQuotes(false);

            std::string input =
                "[section]\n"
                "key1 = foo\n"
                "key2 = \"foo\"\n"
                "key3 =  foo \n"
                "key4 = \" foo \"\n"
                "key5 = \"foo\n"
                "key6 = foo\"\n"
                "key7 =  foo \" foo \n"
                "key8 =  \" foo \" foo \" \n";

            std::string expect =
                "[section]\n"
                "key1 = foo\n"
                "key2 = \"foo\"\n"
                "key3 = foo\n"
                "key4 = \" foo \"\n"
                "key5 = \"foo\n"
                "key6 = foo\"\n"
                "key7 = foo \" foo\n"
                "key8 = \" foo \" foo \"\n";

            DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

            std::string output;
            DOCTEST_CHECK(ini.Save(output) == SI_OK);

            const char* result = ini.GetValue("section", "key1");
            DOCTEST_CHECK(core::stringcmp(result, "foo") == 0);

            result = ini.GetValue("section", "key2");
            DOCTEST_CHECK(core::stringcmp(result, "\"foo\"") == 0);

            result = ini.GetValue("section", "key3");
            DOCTEST_CHECK(core::stringcmp(result, "foo") == 0);

            result = ini.GetValue("section", "key4");
            DOCTEST_CHECK(core::stringcmp(result, "\" foo \"") == 0);

            result = ini.GetValue("section", "key5");
            DOCTEST_CHECK(core::stringcmp(result, "\"foo") == 0);

            result = ini.GetValue("section", "key6");
            DOCTEST_CHECK(core::stringcmp(result, "foo\"") == 0);

            result = ini.GetValue("section", "key7");
            DOCTEST_CHECK(core::stringcmp(result, "foo \" foo") == 0);

            result = ini.GetValue("section", "key8");
            DOCTEST_CHECK(core::stringcmp(result, "\" foo \" foo \"") == 0);

            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            DOCTEST_CHECK(core::stringcmp(expect, output) == 0);
        }
    }
}