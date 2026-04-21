#include "doctest.h"
#include "sw/sensitive_word.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using sensitive_word::char_ignores::special_chars;
using sensitive_word::replace_strategies::chars;
using sensitive_word::result_conditions::always_true;
using sensitive_word::result_conditions::english_word_num_match;
using sensitive_word::sensitive_word_builder;
using sensitive_word::sensitive_word_engine;

TEST_SUITE("sensitive word core")
{
    TEST_CASE("contains find_first find_all replace basic flow")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"五星红旗", "毛主席", "天安门"})
                                           .build();

        const std::string text = "五星红旗迎风飘扬，毛主席的画像屹立在天安门前。";

        CHECK(engine.contains(text));
        CHECK(engine.find_first_word(text).value_or("") == "五星红旗");

        const auto all_words = engine.find_all_words(text);
        REQUIRE(all_words.size() == 3);
        CHECK(all_words[0] == "五星红旗");
        CHECK(all_words[1] == "毛主席");
        CHECK(all_words[2] == "天安门");

        CHECK(engine.replace(text) == "****迎风飘扬，***的画像屹立在***前。");
        CHECK(engine.replace(text, '0') == "0000迎风飘扬，000的画像屹立在000前。");
    }

    TEST_CASE("ignore case and width")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        CHECK(engine.find_first_word("fuCK the bad words.").value_or("") == "fuCK");
        CHECK(engine.find_first_word("ｆｕｃｋ the bad words.").value_or("") == "ｆｕｃｋ");
    }

    TEST_CASE("ignore english style variants")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto words = engine.find_all_words("Ⓕⓤc⒦ the bad words");
        REQUIRE(words.size() == 1);
        CHECK(words[0] == "Ⓕⓤc⒦");
    }

    TEST_CASE("ignore chinese style with opencc")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"五星红旗"})
                                           .build();

        const auto result = engine.find_first("我爱我的祖国和五星紅旗。");
        REQUIRE(result.has_value());
        CHECK(result->word == "五星紅旗");
    }

    TEST_CASE("ignore special chars")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"傻冒", "狗东西"})
                                           .char_ignore(special_chars())
                                           .build();

        const auto words = engine.find_all_words("傻@冒，狗+东西");
        REQUIRE(words.size() == 2);
        CHECK(words[0] == "傻@冒");
        CHECK(words[1] == "狗+东西");
    }

    TEST_CASE("replace covers ignored special chars in raw span")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"傻冒"})
                                           .char_ignore(special_chars())
                                           .build();

        CHECK(engine.replace("你这个傻@冒") == "你这个***");
        CHECK(engine.replace("你这个傻@冒", '#') == "你这个###");
    }

    TEST_CASE("ignore special chars in wrapped html-like text")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"傻逼"})
                                           .char_ignore(special_chars())
                                           .build();

        const auto words = engine.find_all_words("<p>傻逼</p>");
        REQUIRE(words.size() == 1);
        CHECK(words[0] == "傻逼");
    }

    TEST_CASE("english word match is default result condition")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"av"})
                                           .build();

        CHECK_FALSE(engine.contains("I have a nice day。"));

        sensitive_word_engine engine2 = sensitive_word_builder()
                                            .deny_words({"av"})
                                            .result_condition(always_true())
                                            .build();

        CHECK(engine2.contains("I have a nice day。"));
    }

    TEST_CASE("english word num match filters partial numeric hit")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"cp", "69"})
                                           .result_condition(english_word_num_match())
                                           .build();

        const auto words1 = engine.find_all_words("cp cpm trade deficit totaled 695 billion yen");
        REQUIRE(words1.size() == 1);
        CHECK(words1[0] == "cp");

        const auto words2 = engine.find_all_words("cp cpm trade deficit totaled 695 billion yen 69");
        REQUIRE(words2.size() == 2);
        CHECK(words2[0] == "cp");
        CHECK(words2[1] == "69");
    }

    TEST_CASE("num check detects consecutive digits with minimum length")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .num_check_len(8)
                                           .build();

        CHECK_FALSE(engine.contains("订单号 1234567"));
        CHECK(engine.contains("订单号 12345678"));

        const auto result = engine.find_first("订单号 12345678");
        REQUIRE(result.has_value());
        CHECK(result->word == "12345678");
        CHECK(result->normalized_word == "12345678");
        CHECK(result->type == sensitive_word::match_type::num);
    }

    TEST_CASE("num check respects ignored special chars in raw span")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .num_check_len(4)
                                           .char_ignore(special_chars())
                                           .build();

        const auto result = engine.find_first("卡号 12-34 已失效");
        REQUIRE(result.has_value());
        CHECK(result->word == "12-34 ");
        CHECK(result->normalized_word == "1234");
        CHECK(result->type == sensitive_word::match_type::num);
        CHECK(engine.replace("卡号 12-34 已失效") == "卡号 ******已失效");
    }

    TEST_CASE("num check default length and custom length")
    {
        const std::string text = "你懂得：12345678";

        sensitive_word_engine default_engine = sensitive_word_builder()
                                                   .enable_word_check(false)
                                                   .enable_num_check(true)
                                                   .build();
        CHECK(default_engine.find_all_words(text) == std::vector<std::string>{"12345678"});

        sensitive_word_engine custom_len_engine = sensitive_word_builder()
                                                      .enable_word_check(false)
                                                      .enable_num_check(true)
                                                      .num_check_len(9)
                                                      .build();
        CHECK(custom_len_engine.find_all_words(text).empty());
    }

    TEST_CASE("num check supports numeric style normalization")
    {
        const std::string text = "这个是我的微信：9⓿二肆⁹₈③⑸⒋➃㈤㊄";

        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto words = engine.find_all_words(text);
        REQUIRE(words.size() == 1);
        CHECK(words[0] == "9⓿二肆⁹₈③⑸⒋➃㈤㊄");

        const auto result = engine.find_first(text);
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "902498354455");
        CHECK(result->type == sensitive_word::match_type::num);
    }

    TEST_CASE("word matching respects ignore_num_style switch")
    {
        const std::string chinese_text = "花豹用下三滥招式对付疣猪，没想到疣猪居然也有绝招";
        const std::string digit_text = "花豹用下3滥招式对付疣猪，没想到疣猪居然也有绝招";

        sensitive_word_engine engine_true = sensitive_word_builder()
                                                .ignore_num_style(true)
                                                .build();

        engine_true.add_word("下三滥");
        CHECK(engine_true.find_first_word(chinese_text).value_or("") == "下三滥");
        CHECK(engine_true.find_first_word(digit_text).value_or("") == "下3滥");

        engine_true.remove_word("下三滥");
        CHECK_FALSE(engine_true.find_first(chinese_text).has_value());
        CHECK_FALSE(engine_true.find_first(digit_text).has_value());

        sensitive_word_engine engine_true2 = sensitive_word_builder()
                                                 .ignore_num_style(true)
                                                 .build();

        engine_true2.add_word("下3滥");
        CHECK(engine_true2.find_first_word(chinese_text).value_or("") == "下三滥");
        CHECK(engine_true2.find_first_word(digit_text).value_or("") == "下3滥");

        engine_true2.remove_word("下3滥");
        CHECK_FALSE(engine_true2.find_first(chinese_text).has_value());
        CHECK_FALSE(engine_true2.find_first(digit_text).has_value());

        sensitive_word_engine engine_false = sensitive_word_builder()
                                                 .ignore_num_style(false)
                                                 .build();

        engine_false.add_word("下三滥");
        CHECK(engine_false.find_first_word(chinese_text).value_or("") == "下三滥");
        CHECK_FALSE(engine_false.find_first(digit_text).has_value());

        engine_false.remove_word("下三滥");
        CHECK_FALSE(engine_false.find_first(chinese_text).has_value());
        CHECK_FALSE(engine_false.find_first(digit_text).has_value());
    }

    TEST_CASE("allow words have priority over deny words")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"龟孙"})
                                           .allow_words({"龟孙可"})
                                           .word_fail_fast(false)
                                           .build();

        CHECK_FALSE(engine.contains("龟孙可"));
    }

    TEST_CASE("allow words only suppress overlapping region")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"黄片"})
                                           .allow_words({"三黄片"})
                                           .word_fail_fast(false)
                                           .build();

        const auto words = engine.find_all_words("三黄片黄片");
        REQUIRE(words.size() == 1);
        CHECK(words[0] == "黄片");
    }

    TEST_CASE("allow words do not suppress later deny hit")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"政府", "国家", "共产"})
                                           .allow_words({"共产党"})
                                           .word_fail_fast(false)
                                           .build();

        CHECK(engine.find_all_words("共产党是白名单不会被检测").empty());

        const auto words = engine.find_all_words("共产党是白名单不会被检测，但是共产是黑名单");
        REQUIRE(words.size() == 1);
        CHECK(words[0] == "共产");
    }

    TEST_CASE("dynamic add and remove words")
    {
        sensitive_word_engine engine = sensitive_word_builder().build();

        CHECK_FALSE(engine.contains("hello bad world"));

        engine.add_word("bad");
        CHECK(engine.contains("hello bad world"));

        engine.remove_word("bad");
        CHECK_FALSE(engine.contains("hello bad world"));

        engine.add_word("口交");
        CHECK(engine.contains("地铁口交站"));

        engine.add_allow_word("地铁口交站");
        CHECK_FALSE(engine.contains("地铁口交站"));

        engine.remove_allow_word("地铁口交站");
        CHECK(engine.contains("地铁口交站"));
    }

    TEST_CASE("dynamic deny words support repeated hits and removals")
    {
        const std::string text = "测试一下新增敏感词，验证一下删除和新增对不对";

        sensitive_word_engine engine = sensitive_word_builder().build();
        CHECK(engine.find_all_words(text).empty());

        engine.add_word("测试");
        engine.add_word("新增");
        CHECK(
            engine.find_all_words(text) ==
            std::vector<std::string>{"测试", "新增", "新增"});

        engine.remove_word("新增");
        CHECK(engine.find_all_words(text) == std::vector<std::string>{"测试"});

        engine.remove_word("测试");
        CHECK(engine.find_all_words(text).empty());
    }

    TEST_CASE("dynamic allow words take effect immediately")
    {
        const std::string text = "测试一下新增敏感词白名单，验证一下删除和新增对不对";

        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"测试", "新增"})
                                           .word_fail_fast(false)
                                           .build();

        CHECK(
            engine.find_all_words(text) ==
            std::vector<std::string>{"测试", "新增", "新增"});

        engine.add_allow_word("测试");
        engine.add_allow_word("新增");
        CHECK(engine.find_all_words(text).empty());

        engine.remove_allow_word("测试");
        CHECK(engine.find_all_words(text) == std::vector<std::string>{"测试"});

        engine.remove_allow_word("新增");
        CHECK(
            engine.find_all_words(text) ==
            std::vector<std::string>{"测试", "新增", "新增"});
    }

    TEST_CASE("builder loads deny and allow words from text")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .add_deny_words_from_text(
                                               "# comment\n"
                                               "bad\n"
                                               "\n"
                                               "// comment\n"
                                               "evil\n")
                                           .add_allow_words_from_text(
                                               "evil plan\n"
                                               "\n")
                                           .word_fail_fast(false)
                                           .build();

        CHECK(engine.contains("this is bad"));
        CHECK_FALSE(engine.contains("evil plan"));
        CHECK(engine.contains("evil idea"));
    }

    TEST_CASE("builder loads deny and allow words from file")
    {
        const auto deny_file = std::filesystem::path("temp_deny_words.txt");
        const auto allow_file = std::filesystem::path("temp_allow_words.txt");

        {
            std::ofstream deny_out(deny_file);
            deny_out << "# comment\n";
            deny_out << "alpha\n";
            deny_out << "beta\n";
        }

        {
            std::ofstream allow_out(allow_file);
            allow_out << "beta test\n";
        }

        sensitive_word_engine engine = sensitive_word_builder()
                                           .add_deny_words_from_file(deny_file.string())
                                           .add_allow_words_from_file(allow_file.string())
                                           .word_fail_fast(false)
                                           .build();

        CHECK(engine.contains("alpha value"));
        CHECK_FALSE(engine.contains("beta test"));
        CHECK(engine.contains("beta value"));

        std::filesystem::remove(deny_file);
        std::filesystem::remove(allow_file);
    }

    TEST_CASE("fail fast and fail over")
    {
        const std::string text = "我在我的家里玩我的世界";

        sensitive_word_engine fast_engine = sensitive_word_builder()
                                                .deny_words({"我的世界", "我的"})
                                                .word_fail_fast(true)
                                                .build();

        sensitive_word_engine over_engine = sensitive_word_builder()
                                                .deny_words({"我的世界", "我的"})
                                                .word_fail_fast(false)
                                                .build();

        const auto fast_words = fast_engine.find_all_words(text);
        REQUIRE(fast_words.size() == 2);
        CHECK(fast_words[0] == "我的");
        CHECK(fast_words[1] == "我的");

        const auto over_words = over_engine.find_all_words(text);
        REQUIRE(over_words.size() == 2);
        CHECK(over_words[0] == "我的");
        CHECK(over_words[1] == "我的世界");
    }

    TEST_CASE("fail fast and fail over with allow deny interactions")
    {
        sensitive_word_engine fast_engine1 = sensitive_word_builder()
                                                 .deny_words({"操你妈"})
                                                 .allow_words({"你"})
                                                 .word_fail_fast(true)
                                                 .build();
        CHECK(fast_engine1.find_all_words("操你妈") == std::vector<std::string>{"操你妈"});

        sensitive_word_engine fast_engine2 = sensitive_word_builder()
                                                 .deny_words({"大傻逼"})
                                                 .allow_words({"大"})
                                                 .word_fail_fast(true)
                                                 .build();
        CHECK(fast_engine2.find_all_words("大傻逼") == std::vector<std::string>{"大傻逼"});

        sensitive_word_engine fast_engine3 = sensitive_word_builder()
                                                 .deny_words({"口交"})
                                                 .allow_words({"地铁口交易"})
                                                 .word_fail_fast(true)
                                                 .build();
        CHECK(fast_engine3.find_all_words("地铁口交易").empty());

        sensitive_word_engine over_engine1 = sensitive_word_builder()
                                                 .deny_words({"操你妈"})
                                                 .allow_words({"你"})
                                                 .word_fail_fast(false)
                                                 .build();
        CHECK(over_engine1.find_all_words("操你妈") == std::vector<std::string>{"操你妈"});

        sensitive_word_engine over_engine2 = sensitive_word_builder()
                                                 .deny_words({"大傻逼"})
                                                 .allow_words({"大"})
                                                 .word_fail_fast(false)
                                                 .build();
        CHECK(over_engine2.find_all_words("大傻逼") == std::vector<std::string>{"大傻逼"});

        sensitive_word_engine over_engine3 = sensitive_word_builder()
                                                 .deny_words({"口交"})
                                                 .allow_words({"地铁口交易"})
                                                 .word_fail_fast(false)
                                                 .build();
        CHECK(over_engine3.find_all_words("地铁口交易").empty());
    }

    TEST_CASE("fail over prefers longer later match at same text")
    {
        const std::string text = "他的世界它的世界和她的世界都不是我的也不是我的世界";

        sensitive_word_engine over_engine = sensitive_word_builder()
                                                .deny_words({"我的世界", "我的"})
                                                .word_fail_fast(false)
                                                .build();
        CHECK(
            over_engine.find_all_words(text) ==
            std::vector<std::string>{"我的", "我的世界"});

        sensitive_word_engine fast_engine = sensitive_word_builder()
                                                .deny_words({"我的世界", "我的"})
                                                .build();
        CHECK(
            fast_engine.find_all_words(text) ==
            std::vector<std::string>{"我的", "我的"});
    }

    TEST_CASE("fail over no false positive on near miss")
    {
        const std::string text = "他的世界它的世界和她的世界都不是我的也不是我的天界";

        sensitive_word_engine over_engine = sensitive_word_builder()
                                                .deny_words({"我的世界"})
                                                .word_fail_fast(false)
                                                .build();
        CHECK(over_engine.find_all_words(text).empty());

        sensitive_word_engine fast_engine = sensitive_word_builder()
                                                .deny_words({"我的世界"})
                                                .build();
        CHECK(fast_engine.find_all_words(text).empty());
    }

    TEST_CASE("builder default replace strategy is respected")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"毛主席"})
                                           .replace_strategy(chars('#'))
                                           .build();

        CHECK(engine.replace("毛主席的画像") == "###的画像");
    }

    TEST_CASE("ignore repeat with english style")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .ignore_repeat(true)
                                           .build();

        const auto result = engine.find_first("ⒻⒻⒻfⓤuⓤ⒰cⓒ⒦ the bad words");
        REQUIRE(result.has_value());
        CHECK(result->word == "ⒻⒻⒻfⓤuⓤ⒰cⓒ⒦");
    }
}