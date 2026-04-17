#include "doctest.h"
#include "opencc.h"

#include <cstring>
#include <string>
#include <vector>
#include <filesystem>

/**
 * 为 OpenCC 构造“附加搜索路径”。
 *
 * OpenCC 的 SimpleConverter 支持：
 *   SimpleConverter(configFileName, paths)
 *
 * 其中 paths 用于告诉 OpenCC 去哪里找：
 *   - 配置文件（json）
 *   - 词典文件（ocd2）
 *
 * 这样测试就不依赖“当前工作目录必须正好是某个特定目录”。
 */
static std::vector<std::string> opencc_search_paths(void)
{
    const auto root = std::filesystem::path(".");
    return {
        (root / "data" / "config").string(),
        (root / "data" / "dictionary").string(),
    };
}

/**
 * 统一封装一个创建转换器的小助手，便于多个测试复用。
 */
static opencc::SimpleConverter make_converter(const std::string& config_name)
{
    return opencc::SimpleConverter(config_name, opencc_search_paths());
}

TEST_SUITE("opencc cpp usage")
{
    TEST_CASE("SimpleConverter: 使用 s2t.json 完成基础简转繁")
    {
        /*
        * 这是 OpenCC 最常见的 C++ 用法：
        *
        *   opencc::SimpleConverter converter("s2t.json");
        *   std::string result = converter.Convert("汉字");
        *
        * 在本项目测试中，为了避免路径问题，我们额外传入搜索路径。
        */
        const auto converter = make_converter("s2t.json");

        const std::string input  = "汉字";
        const std::string output = converter.Convert(input);

        CHECK(output == "漢字");
    }

    TEST_CASE("SimpleConverter: 使用 t2s.json 完成基础繁转简")
    {
        /*
        * 这个测试与上一个方向相反，用来验证：
        *   繁体 -> 简体
        *
        * 这也是在敏感词系统里常见的文本归一化方式：
        * 先把输入统一转成简体，再进行词库匹配。
        */
        const auto converter = make_converter("t2s.json");

        const std::string input  = "漢字";
        const std::string output = converter.Convert(input);

        CHECK(output == "汉字");
    }

    TEST_CASE("SimpleConverter: 演示不同 Convert 重载的基本用法")
    {
        /*
        * SimpleConverter 提供多个 Convert 重载：
        *   1. Convert(const std::string&)
        *   2. Convert(const char*)
        *   3. Convert(const char*, size_t)
        *   4. Convert(const char*, char* output)
        *   5. Convert(const char*, size_t, char* output)
        *
        * 这个测试把常用几种方式都覆盖到，便于后续复制到业务代码里。
        */
        const auto converter = make_converter("s2t.json");

        SUBCASE("Convert(std::string)")
        {
            const std::string input  = "汉字";
            const std::string output = converter.Convert(input);
            CHECK(output == "漢字");
        }

        SUBCASE("Convert(const char*)")
        {
            const char*       input  = "汉字";
            const std::string output = converter.Convert(input);
            CHECK(output == "漢字");
        }

        SUBCASE("Convert(const char*, size_t): 只转换前 2 个汉字")
        {
            /*
            * 这里输入是 UTF-8 文本：
            *   "汉字abc"
            *
            * “汉字”两个汉字在 UTF-8 下通常占 6 个字节，
            * 所以后面只取前 6 个字节用于转换，"abc" 不参与。
            */
            const char*       input  = "汉字abc";
            const std::string output = converter.Convert(input, 6);
            CHECK(output == "漢字");
        }

        SUBCASE("Convert(const char*, char* output): 写入调用方提供的缓冲区")
        {
            /*
            * 这种形式适合调用方已经有预分配缓冲区的场景。
            *
            * 注意：
            * - output 缓冲区需要足够大
            * - 返回值是“转换后文本的字节长度”
            * - 对于 UTF-8 文本，字节长度不一定等于字符数
            */
            char        buffer[64] = {0};
            const char* input      = "汉字";
            const auto  written    = converter.Convert(input, buffer);

            CHECK(written == std::strlen("漢字"));
            CHECK(std::string(buffer, written) == "漢字");
        }

        SUBCASE("Convert(const char*, size_t, char* output): 指定输入长度并输出到缓冲区")
        {
            char        buffer[64] = {0};
            const char* input      = "汉字abc";
            const auto  written    = converter.Convert(input, 6, buffer);

            CHECK(written == std::strlen("漢字"));
            CHECK(std::string(buffer, written) == "漢字");
        }
    }

    TEST_CASE("SimpleConverter: 演示地区词汇转换 s2twp.json")
    {
        /*
        * s2twp.json 不只是“简体转繁体”，还会尽量转换为台湾常用词。
        *
        * README 里提到的典型示例之一就是：
        *   鼠标 -> 滑鼠
        *
        * 这类配置很适合用于：
        * - 多地区文本标准化
        * - 面向台湾/香港用户的展示层转换
        */
        const auto converter = make_converter("s2twp.json");

        const std::string input  = "鼠标";
        const std::string output = converter.Convert(input);

        CHECK(output == "滑鼠");
    }

    TEST_CASE("SimpleConverter: 适合敏感词项目的“先归一化再匹配”思路示例")
    {
        /*
        * 这个测试不只是验证 API，
        * 也在演示 OpenCC 在“敏感词系统”中的一个典型使用方式：
        *
        *   原始输入（可能是繁体） --OpenCC归一化--> 统一文本（简体）
        *   再用统一文本与敏感词词库进行匹配
        *
        * 这样可以减少维护多套词库的成本。
        */
        const auto normalize_to_simplified = make_converter("t2s.json");

        const std::string user_input = "這是一段包含漢字的文本";
        const std::string normalized = normalize_to_simplified.Convert(user_input);

        CHECK(normalized == "这是一段包含汉字的文本");

        /*
        * 模拟后续敏感词检测或关键词检测前的统一化结果。
        * 这里只做一个简单的 contains 检查，用来表达“归一化后的文本更容易统一匹配”。
        */
        CHECK(normalized.find("汉字") != std::string::npos);
    }

    TEST_CASE("SimpleConverter: 双向转换可以构成一个简单回环示例")
    {
        /*
        * 在一些业务中，可能会：
        *   1. 先把文本统一转成简体做内部处理
        *   2. 再根据展示场景转成繁体
        *
        * 这个测试演示一个简单回环：
        *   简体 --s2t--> 繁体 --t2s--> 简体
        */
        const auto s2t = make_converter("s2t.json");
        const auto t2s = make_converter("t2s.json");

        const std::string original    = "汉字转换测试";
        const std::string traditional = s2t.Convert(original);
        const std::string simplified  = t2s.Convert(traditional);

        CHECK(traditional == "漢字轉換測試");
        CHECK(simplified  == original);
    }
}