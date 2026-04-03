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

    // 快速入门导览：
    // 1) 从字符串加载配置
    // 2) 查询分区/键/值
    // 3) 获取所有分区、键、多值
    // 4) 修改与删除
    // 5) 保存并校验结果
    DOCTEST_TEST_CASE("QuickStartGuide")
    {
        const char* input =
            "; 全局注释：应用配置\n"
            "[server]\n"
            "host = 127.0.0.1\n"
            "port = 8080\n"
            "enabled = true\n"
            "\n"
            "[features]\n"
            "name = alpha\n"
            "name = beta\n"
            "timeout_ms = 1500\n";

        // 总是设置为 utf-8 编码
        CSimpleIni ini(true);

        // 允许同名 key（用于演示多值）
        ini.SetMultiKey(true);

        // ---------- 1) 加载 ----------
        DOCTEST_CHECK(ini.LoadData(input) == SI_OK);

        // ---------- 2) 基础读取 ----------
        DOCTEST_CHECK(ini.SectionExists("server"));
        DOCTEST_CHECK(ini.KeyExists("server", "host"));
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("server", "host", ""), "127.0.0.1") == 0);
        DOCTEST_CHECK(ini.GetLongValue("server", "port", 0) == 8080);
        DOCTEST_CHECK(ini.GetBoolValue("server", "enabled", false));
        DOCTEST_CHECK(ini.GetLongValue("features", "timeout_ms", 0) == 1500);

        // 不存在时返回默认值（快速定位“兜底逻辑”）
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("server", "missing", "default-v"), "default-v") == 0);

        // ---------- 3) 枚举 sections / keys ----------
        CSimpleIni::TNamesDepend sections;
        ini.GetAllSections(sections);
        DOCTEST_CHECK(sections.size() == 2);

        CSimpleIni::TNamesDepend serverKeys;
        ini.GetAllKeys("server", serverKeys);
        DOCTEST_CHECK(serverKeys.size() == 3);

        // ---------- 4) 多值读取 ----------
        bool hasMulti = false;
        const char* firstName = ini.GetValue("features", "name", "", &hasMulti);
        DOCTEST_CHECK(core::stringcmp(firstName, "alpha") == 0);
        DOCTEST_CHECK(hasMulti);

        CSimpleIni::TNamesDepend allNames;
        ini.GetAllValues("features", "name", allNames);
        allNames.sort(CSimpleIni::Entry::LoadOrder());

        DOCTEST_CHECK(allNames.size() == 2);
        auto it = allNames.begin();
        DOCTEST_CHECK(core::stringcmp(it->pItem, "alpha") == 0);
        ++it;
        DOCTEST_CHECK(core::stringcmp(it->pItem, "beta") == 0);

        // ---------- 5) 修改（增/改） ----------
        // 注意：当前开启了 MultiKey，默认 SetXXX 会“追加同名 key”而不是覆盖。
        // 因此这里传 a_bForceReplace=true，演示“在多值模式下进行覆盖更新”。
        DOCTEST_CHECK(ini.SetValue("server", "host", "0.0.0.0", nullptr, true) == SI_UPDATED);
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("server", "host", ""), "0.0.0.0") == 0);

        DOCTEST_CHECK(ini.SetLongValue("server", "port", 9090, nullptr, false, true) == SI_UPDATED);
        DOCTEST_CHECK(ini.GetLongValue("server", "port", 0) == 9090);

        DOCTEST_CHECK(ini.SetBoolValue("server", "enabled", false, nullptr, true) == SI_UPDATED);
        DOCTEST_CHECK(!ini.GetBoolValue("server", "enabled", true));

        DOCTEST_CHECK(ini.SetValue("server", "new_key", "hello") == SI_INSERTED);
        DOCTEST_CHECK(core::stringcmp(ini.GetValue("server", "new_key", ""), "hello") == 0);

        // ---------- 6) 删除 ----------
        DOCTEST_CHECK(ini.Delete("server", "new_key"));
        DOCTEST_CHECK(!ini.KeyExists("server", "new_key"));

        // 删除 features 分区里的全部内容
        DOCTEST_CHECK(ini.Delete("features", nullptr));
        DOCTEST_CHECK(!ini.SectionExists("features"));

        // ---------- 7) 保存 ----------
        std::string output;
        DOCTEST_CHECK(ini.Save(output) == SI_OK);

        // 跨平台换行统一处理，便于稳定断言
        output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());

        // 结果中应该包含修改后的关键配置
        DOCTEST_CHECK(core::contains(output, "[server]\n"));
        DOCTEST_CHECK(core::contains(output, "host = 0.0.0.0\n"));
        DOCTEST_CHECK(core::contains(output, "port = 9090\n"));
        DOCTEST_CHECK(core::contains(output, "enabled = false\n"));

        // 已删除的分区不应再出现
        DOCTEST_CHECK(!core::contains(output, "[features]\n"));
    }
}