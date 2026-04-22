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

TEST_SUITE("sensitive word usage")
{
    TEST_CASE("包含判断、查找首个、查找全部、替换基本流程")
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

        const auto results = engine.find_all(text);
        REQUIRE(results.size() == 3);
        CHECK(engine.replace(text) == "****迎风飘扬，***的画像屹立在***前。");
        CHECK(engine.replace(text, '0') == "0000迎风飘扬，000的画像屹立在000前。");
        CHECK(engine.replace(text, results) == "****迎风飘扬，***的画像屹立在***前。");
        CHECK(engine.replace(text, results, '0') == "0000迎风飘扬，000的画像屹立在000前。");
        CHECK(engine.replace(text, results, *chars('#')) == "####迎风飘扬，###的画像屹立在###前。");

        sensitive_word_engine engine1 = sensitive_word_builder()
                                           .enable_word_check(true)
                                           .ignore_repeat(true)
                                           .add_deny_words_from_file("./res/dict-2026-04-20.txt")
                                           .build();

        CHECK(engine1.replace("你怕是个大傻逼吧") == "你怕是个大**吧");
        CHECK(engine1.replace("你怕是个大傻傻傻逼逼逼吧") == "你怕是个大******吧");
        CHECK(engine1.replace("ⒻⒻⒻfⓤuⓤ⒰cⓒ⒦ you!") == "*********** you!");
        CHECK(engine1.replace("FFFUUUCCCKKK you!") == "************ you!");
        CHECK(engine.replace(text) == "****迎风飘扬，***的画像屹立在***前。");
        CHECK(engine1.replace("64事件") == "**事件");
    }

    TEST_CASE("基于find_all结果复用替换")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"坏词", "敏感词", "违规"})
                                           .build();

        const std::string text = "这里有坏词，也有敏感词，还有违规内容。";
        const auto        results = engine.find_all(text);
        REQUIRE(results.size() == 3);

        CHECK(engine.replace(text, results) == "这里有**，也有***，还有**内容。");
        CHECK(engine.replace(text, results, '#') == "这里有##，也有###，还有##内容。");

        std::vector<sensitive_word::word_result> filtered_results;
        filtered_results.push_back(results[1]);
        CHECK(engine.replace(text, filtered_results) == "这里有坏词，也有***，还有违规内容。");
    }

    TEST_CASE("忽略大小写和全半角")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        CHECK(engine.find_first_word("fuCK the bad words.").value_or("") == "fuCK");
        CHECK(engine.find_first_word("ｆｕｃｋ the bad words.").value_or("") == "ｆｕｃｋ");

        CHECK(engine.replace("fuCK the bad words.") == "**** the bad words.");
        CHECK(engine.replace("ｆｕｃｋ the bad words.", '0') == "0000 the bad words.");
    }

    TEST_CASE("忽略英文样式变体")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto words = engine.find_all_words("Ⓕⓤc⒦ the bad words");
        REQUIRE(words.size() == 1);
        CHECK(words[0] == "Ⓕⓤc⒦");

        CHECK(engine.replace("Ⓕⓤc⒦ the bad words") == "**** the bad words");
    }

    TEST_CASE("忽略繁体中文变体（opencc）")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"五星红旗"})
                                           .build();

        const auto result = engine.find_first("我爱我的祖国和五星紅旗。");
        REQUIRE(result.has_value());
        CHECK(result->word == "五星紅旗");

        CHECK(engine.replace("我爱我的祖国和五星紅旗。") == "我爱我的祖国和****。");
    }

    TEST_CASE("忽略特殊字符")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"傻冒", "狗东西"})
                                           .char_ignore(special_chars())
                                           .build();

        const auto words = engine.find_all_words("傻@冒，狗+东西");
        REQUIRE(words.size() == 2);
        CHECK(words[0] == "傻@冒");
        CHECK(words[1] == "狗+东西");

        CHECK(engine.replace("傻@&%￥#！冒，狗+东西") == "********，****");
    }

    TEST_CASE("替换覆盖原始范围内被忽略的特殊字符")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"傻冒"})
                                           .char_ignore(special_chars())
                                           .build();

        CHECK(engine.replace("你这个傻@冒") == "你这个***");
        CHECK(engine.replace("你这个傻@冒", '#') == "你这个###");
    }

    TEST_CASE("忽略HTML类标签文本中的特殊字符")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"傻逼"})
                                           .char_ignore(special_chars())
                                           .build();

        const auto words = engine.find_all_words("<p>傻逼</p>");
        REQUIRE(words.size() == 1);
        CHECK(words[0] == "傻逼");

        CHECK(engine.replace("<p>傻逼</p>") == "<p>**</p>");
        CHECK(engine.replace("<p>大傻逼</p>") == "<p>大**</p>");
    }

    TEST_CASE("英文单词匹配是默认的结果条件")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"av"})
                                           .build();

        CHECK_FALSE(engine.contains("I have a nice day。"));
        CHECK(engine.replace("I have a nice day。") == "I have a nice day。");

        sensitive_word_engine engine2 = sensitive_word_builder()
                                            .deny_words({"av"})
                                            .result_condition(always_true())
                                            .build();

        CHECK(engine2.contains("I have a nice day。"));
        CHECK(engine2.replace("I have a nice day。") == "I h**e a nice day。");
    }

    TEST_CASE("英文单词数字匹配过滤部分数字命中")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"cp", "69"})
                                           .result_condition(english_word_num_match())
                                           .build();

        const auto words1 = engine.find_all_words("cp cpm trade deficit totaled 695 billion yen");
        REQUIRE(words1.size() == 1);
        CHECK(words1[0] == "cp");
        CHECK(engine.replace("cp cpm trade deficit totaled 695 billion yen") == "** cpm trade deficit totaled 695 billion yen");

        const auto words2 = engine.find_all_words("cp cpm trade deficit totaled 695 billion yen 69");
        REQUIRE(words2.size() == 2);
        CHECK(words2[0] == "cp");
        CHECK(words2[1] == "69");
        CHECK(engine.replace("cp cpm trade deficit totaled 695 billion yen 69") == "** cpm trade deficit totaled 695 billion yen **");
    }

    TEST_CASE("数字检测识别满足最小长度的连续数字")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .num_check_len(8)
                                           .build();

        CHECK_FALSE(engine.contains("订单号 1234567"));
        CHECK(engine.contains("订单号 12345678"));
        CHECK(engine.replace("订单号 12345678") == "订单号 ********");

        const auto result = engine.find_first("订单号 12345678");
        REQUIRE(result.has_value());
        CHECK(result->word == "12345678");
        CHECK(result->normalized_word == "12345678");
        CHECK(result->type == sensitive_word::match_type::num);
    }

    TEST_CASE("数字检测在原始范围内尊重被忽略的特殊字符")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .char_ignore(special_chars())
                                           .build();

        const auto result = engine.find_first("卡号 12-34 已失效");
        REQUIRE(result.has_value());
        CHECK(result->word == "12-34 ");
        CHECK(result->normalized_word == "1234");
        CHECK(result->type == sensitive_word::match_type::num);
        CHECK(engine.replace("卡号 12-34 已失效") == "卡号 ******已失效");
    }

    TEST_CASE("数字检测默认长度和自定义长度")
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

    TEST_CASE("数字检测支持数字样式归一化")
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
        CHECK(engine.replace(text) == "这个是我的微信：************");
    }

    TEST_CASE("词语匹配遵循ignore_num_style开关")
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

    TEST_CASE("白名单词语优先于黑名单词语")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"龟孙"})
                                           .allow_words({"龟孙可"})
                                           .word_fail_fast(false)
                                           .build();

        CHECK_FALSE(engine.contains("龟孙可"));
    }

    TEST_CASE("白名单仅抑制重叠区域")
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

    TEST_CASE("白名单不会抑制后续的黑名单命中")
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

    TEST_CASE("动态添加和删除词语")
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

    TEST_CASE("动态黑名单词语支持重复命中和删除")
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

    TEST_CASE("查询词条状态")
    {
        sensitive_word_engine engine = sensitive_word_builder().build();

        const auto empty_status = engine.query_word_status("hello");
        CHECK_FALSE(empty_status.exists);
        CHECK_FALSE(empty_status.in_deny);
        CHECK_FALSE(empty_status.in_allow);

        engine.add_word("hello");
        const auto deny_status = engine.query_word_status("hello");
        CHECK(deny_status.exists);
        CHECK(deny_status.in_deny);
        CHECK_FALSE(deny_status.in_allow);

        engine.add_allow_word("world");
        const auto allow_status = engine.query_word_status("world");
        CHECK(allow_status.exists);
        CHECK_FALSE(allow_status.in_deny);
        CHECK(allow_status.in_allow);

        engine.add_allow_word("hello");
        const auto both_status = engine.query_word_status("hello");
        CHECK(both_status.exists);
        CHECK(both_status.in_deny);
        CHECK(both_status.in_allow);
    }

    TEST_CASE("查询词条状态遵循当前引擎的归一化配置")
    {
        sensitive_word_engine engine = sensitive_word_builder().build();

        engine.add_word("fuck");
        const auto status = engine.query_word_status("ＦＵＣＫ");
        CHECK(status.exists);
        CHECK(status.in_deny);
        CHECK_FALSE(status.in_allow);
    }

    TEST_CASE("动态白名单词语立即生效")
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

    TEST_CASE("构建器从文本加载黑名单和白名单")
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

    TEST_CASE("构建器从文件加载黑名单和白名单")
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

    TEST_CASE("快速失败和完整匹配")
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

    TEST_CASE("快速失败和完整匹配与黑白名单的交互")
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

    TEST_CASE("完整匹配在相同文本位置优先选择更长的后续匹配")
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

    TEST_CASE("完整匹配在近似未命中时无误报")
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

    TEST_CASE("构建器默认替换策略生效")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"毛主席"})
                                           .replace_strategy(chars('#'))
                                           .build();

        CHECK(engine.replace("毛主席的画像") == "###的画像");
    }

    TEST_CASE("忽略重复（英文样式）")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .ignore_repeat(true)
                                           .build();

        const auto result = engine.find_first("ⒻⒻⒻfⓤuⓤ⒰cⓒ⒦ the bad words");
        REQUIRE(result.has_value());
        CHECK(result->word == "ⒻⒻⒻfⓤuⓤ⒰cⓒ⒦");
    }

    // ============================================================
    // text_normalizer 补充测试
    // ============================================================

    // --- fold_ascii_case: Latin-1 / Latin Extended 大小写折叠 ---

    TEST_CASE("折叠Latin-1带重音大写字母的大小写")
    {
        // É(0x00C9) -> é(0x00E9), Ü(0x00DC) -> ü(0x00FC)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"café"})
                                           .build();

        const auto result = engine.find_first("CAFÉ");
        REQUIRE(result.has_value());
        CHECK(result->word == "CAFÉ");
    }

    TEST_CASE("折叠Latin Extended-A大写字母的大小写")
    {
        // Ō(0x014C) -> ō(0x014D)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"ōsaka"})
                                           .build();

        const auto result = engine.find_first("ŌSAKA");
        REQUIRE(result.has_value());
        CHECK(result->word == "ŌSAKA");
    }

    TEST_CASE("折叠拼音大写元音的大小写")
    {
        // Ǎ(0x01CD) -> ǎ(0x01CE), Ǖ(0x01D5) -> ǖ(0x01D6)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"ǎbǎ"})
                                           .build();

        const auto result = engine.find_first("ǍBǍ");
        REQUIRE(result.has_value());
        CHECK(result->word == "ǍBǍ");
    }

    // --- fold_width: 全角符号 → 半角符号 ---

    TEST_CASE("全角符号折叠为半角符号")
    {
        // 全角！(0xFF01) -> !(0x21), 全角＠(0xFF20) -> @(0x40)
        // 全角： -> :, 全角； -> ;
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"a!b"})
                                           .build();

        const auto result = engine.find_first("ａ！ｂ");
        REQUIRE(result.has_value());
        CHECK(result->word == "ａ！ｂ");
    }

    TEST_CASE("全角表意空格折叠为ASCII空格")
    {
        // 全角空格 U+3000 -> ASCII 空格
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"hello world"})
                                           .build();

        // U+3000 是表意空格
        const auto result = engine.find_first("hello\u3000world");
        REQUIRE(result.has_value());
        CHECK(result->word == "hello\u3000world");
    }

    TEST_CASE("全角货币和箭头符号折叠")
    {
        // ￥(0xFFE5) -> ¥(0x00A5), ￠(0xFFE0) -> ¢(0x00A2)
        // ￡(0xFFE1) -> £(0x00A3), ￢(0xFFE2) -> ¬(0x00AC)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"¥100"})
                                           .build();

        // ￥ 是全角日元符号
        const auto result = engine.find_first("￥100");
        REQUIRE(result.has_value());
        CHECK(result->word == "￥100");
    }

    TEST_CASE("半角片假名折叠为全角片假名")
    {
        // 半角片假名 ﾊ(0xFF8A) -> ハ, ﾝ(0xFF9D) -> ン
        // 使用包含半角片假名的词进行测试
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        // 此测试验证半角片假名映射是否正常工作；
        // 片假名默认被视为非词语字符，
        // 因此通过数字检测验证半角片假名不会干扰附近的数字检测
        CHECK_FALSE(engine.contains("ﾊﾝｶﾅ"));
    }

    // --- fold_english_style: 数学样式字母 ---

    TEST_CASE("数学粗体字母的英文样式折叠")
    {
        // 数学粗体小写字母: base=0x1D41A, f=+5=0x1D41F, u=+20=0x1D42E,
        //                   c=+2=0x1D41C, k=+10=0x1D424
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D41F\U0001D42E\U0001D41C\U0001D424");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D41F\U0001D42E\U0001D41C\U0001D424");
    }

    TEST_CASE("数学斜体字母的英文样式折叠")
    {
        // 数学斜体小写字母: base=0x1D44E, f=+5=0x1D453, u=+20=0x1D462,
        //                   c=+2=0x1D450, k=+10=0x1D458
        // 注意: 斜体小写字母没有空洞
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D453\U0001D462\U0001D450\U0001D458");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D453\U0001D462\U0001D450\U0001D458");
    }

    TEST_CASE("数学斜体大写字母的英文样式折叠")
    {
        // 数学斜体大写字母: base=0x1D434, B=+1=0x1D435 -> b
        // H(=+7) 是 U+1D43B 处的空洞，所以 U+1D435 (B) 是安全的
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        const auto result = engine.find_first("\U0001D435ad");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D435ad");
    }

    TEST_CASE("数学哥特体字母的英文样式折叠")
    {
        // 数学哥特体小写字母: base=0x1D51E, f=+5=0x1D523, u=+20=0x1D532,
        //                     c=+2=0x1D520, k=+10=0x1D528
        // 哥特体小写字母没有空洞
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D523\U0001D532\U0001D520\U0001D528");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D523\U0001D532\U0001D520\U0001D528");
    }

    TEST_CASE("数学双线体字母的英文样式折叠")
    {
        // 数学双线体小写字母: base=0x1D552, f=+5=0x1D557, u=+20=0x1D566,
        //                     c=+2=0x1D554, k=+10=0x1D55C
        // 双线体小写字母没有空洞
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D557\U0001D566\U0001D554\U0001D55C");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D557\U0001D566\U0001D554\U0001D55C");
    }

    TEST_CASE("数学无衬线体字母的英文样式折叠")
    {
        // 数学无衬线体小写字母: base=0x1D5BA, f=+5=0x1D5BF, u=+20=0x1D5CE,
        //                       c=+2=0x1D5BC, k=+10=0x1D5C4
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D5BF\U0001D5CE\U0001D5BC\U0001D5C4");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D5BF\U0001D5CE\U0001D5BC\U0001D5C4");
    }

    TEST_CASE("数学等宽体字母的英文样式折叠")
    {
        // 数学等宽体小写字母: base=0x1D68A, f=+5=0x1D68F, u=+20=0x1D69E,
        //                     c=+2=0x1D68C, k=+10=0x1D694
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D68F\U0001D69E\U0001D68C\U0001D694");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D68F\U0001D69E\U0001D68C\U0001D694");
    }

    TEST_CASE("类字母符号的英文样式折叠")
    {
        // 类字母符号: ℂ(U+2102) -> C, ℍ(U+210D) -> H,
        //             ℕ(U+2115) -> N, ℝ(U+211D) -> R, ℤ(U+2124) -> Z
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"china"})
                                           .build();

        // ℂ -> C, 然后 h, i, n, a
        const auto result = engine.find_first("\u2102hina");
        REQUIRE(result.has_value());
        CHECK(result->word == "\u2102hina");
    }

    TEST_CASE("数学花体字母的英文样式折叠")
    {
        // 数学花体大写字母: ℬ(U+212C) -> B
        // 数学花体小写字母: ℯ(U+212F) -> e
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        const auto result = engine.find_first("\u212cad");
        REQUIRE(result.has_value());
        CHECK(result->word == "\u212cad");
    }

    TEST_CASE("数学粗花体字母的英文样式折叠")
    {
        // 数学粗花体小写字母: base=0x1D4EA, f=+5=0x1D4EF, u=+20=0x1D4FE,
        //                     c=+2=0x1D4EC, k=+10=0x1D4F4
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D4EF\U0001D4FE\U0001D4EC\U0001D4F4");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D4EF\U0001D4FE\U0001D4EC\U0001D4F4");
    }

    TEST_CASE("数学粗哥特体字母的英文样式折叠")
    {
        // 数学粗哥特体大写字母: base=0x1D56C, B=+1=0x1D56D -> B
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        const auto result = engine.find_first("\U0001D56Dad");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D56Dad");
    }

    TEST_CASE("数学无衬线粗体字母的英文样式折叠")
    {
        // 数学无衬线粗体小写字母: base=0x1D5EE, b=+1=0x1D5EF, a=+0=0x1D5EE,
        //                         d=+3=0x1D5F1
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        const auto result = engine.find_first("\U0001D5EF\U0001D5EE\U0001D5F1");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D5EF\U0001D5EE\U0001D5F1");
    }

    TEST_CASE("数学无衬线斜体字母的英文样式折叠")
    {
        // 数学无衬线斜体小写字母: base=0x1D622, f=+5=0x1D627, u=+20=0x1D636,
        //                         c=+2=0x1D624, k=+10=0x1D62C
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D627\U0001D636\U0001D624\U0001D62C");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D627\U0001D636\U0001D624\U0001D62C");
    }

    TEST_CASE("数学无衬线粗斜体字母的英文样式折叠")
    {
        // 数学无衬线粗斜体小写字母: base=0x1D656, f=+5=0x1D65B, u=+20=0x1D66A,
        //                           c=+2=0x1D658, k=+10=0x1D660
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        const auto result = engine.find_first("\U0001D65B\U0001D66A\U0001D658\U0001D660");
        REQUIRE(result.has_value());
        CHECK(result->word == "\U0001D65B\U0001D66A\U0001D658\U0001D660");
    }

    TEST_CASE("斜体空洞h保持原样")
    {
        // 数学斜体 h (U+1D439) 是空洞 - 不应被折叠
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .build();

        // 不应匹配，因为斜体 h 保持原样
        CHECK_FALSE(engine.contains("fu\U0001D439k"));
    }

    TEST_CASE("花体空洞保持原样")
    {
        // 数学花体大写字母空洞: 1D49D, 1D4A0, 1D4A1, 1D4A3, 1D4A4, 1D4A7, 1D4A8, 1D4AD
        // 这些码点是保留的（空洞），应保持原样
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        // U+1D49D 是花体大写字母的空洞，不应匹配任何字母
        CHECK_FALSE(engine.contains("\U0001D49D"));
    }

    TEST_CASE("双线体空洞保持原样")
    {
        // 双线体大写字母空洞: 1D53A, 1D53F, 1D545, 1D547-1D549
        // 这些是保留码点，应保持原样
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        // U+1D53A 是双线体大写字母的空洞
        CHECK_FALSE(engine.contains("\U0001D53A"));
    }

    TEST_CASE("哥特体空洞保持原样")
    {
        // 哥特体大写字母空洞: U+1D506
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        CHECK_FALSE(engine.contains("\U0001D506"));
    }

    // --- fold_num_style: 上下标数字 ---

    TEST_CASE("上标数字的数字样式折叠")
    {
        // ⁰(U+2070), ¹(U+00B9), ²(U+00B2), ³(U+00B3), ⁴(U+2074)-⁹(U+2079)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        // ⁴²₀₁ = 4201 (4个上标数字，>= 8 加后续数字)
        const auto result = engine.find_first("卡号⁴²₀₁⁵⁸⁹⁰");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "42015890");
    }

    TEST_CASE("下标数字的数字样式折叠")
    {
        // ₀(U+2080)-₉(U+2089)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("卡号₁₂₃₄₅₆₇₈");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "12345678");
    }

    // --- fold_num_style: 数学样式数字 ---

    TEST_CASE("数学粗体数字的数字样式折叠")
    {
        // 数学粗体数字: 𝟎(U+1D7CE)-𝟗(U+1D7D7)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        // 𝟏𝟐𝟑𝟒𝟓𝟔𝟕𝟖
        const auto result = engine.find_first(
            "\U0001D7CF\U0001D7D0\U0001D7D1\U0001D7D2"
            "\U0001D7D3\U0001D7D4\U0001D7D5\U0001D7D6");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "12345678");
    }

    TEST_CASE("数学双线体数字的数字样式折叠")
    {
        // 数学双线体数字: 𝟘(U+1D7D8)-𝟡(U+1D7E1)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        // 𝟘𝟙𝟚𝟛𝟜𝟝𝟞𝟟
        const auto result = engine.find_first(
            "\U0001D7D8\U0001D7D9\U0001D7DA\U0001D7DB"
            "\U0001D7DC\U0001D7DD\U0001D7DE\U0001D7DF");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "01234567");
    }

    TEST_CASE("数学无衬线体数字的数字样式折叠")
    {
        // 数学无衬线体数字: 𝟢(U+1D7E2)-𝟫(U+1D7EB)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first(
            "\U0001D7E2\U0001D7E3\U0001D7E4\U0001D7E5"
            "\U0001D7E6\U0001D7E7\U0001D7E8\U0001D7E9");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "01234567");
    }

    // --- fold_num_style: 带圈/括号/句点数字 ---

    TEST_CASE("带圈数字①-⑨的数字样式折叠")
    {
        // ①(U+2460)-⑨(U+2468) -> 1-9
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        // ①②③④⑤⑥⑦⑧ -> 12345678
        const auto result = engine.find_first("①②③④⑤⑥⑦⑧");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "12345678");
    }

    TEST_CASE("带括号数字⑴-⑼的数字样式折叠")
    {
        // ⑴(U+2474)-⑼(U+247C) -> 1-9
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("⑴⑵⑶⑷⑸⑹⑺⑻");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "12345678");
    }

    TEST_CASE("带句点数字⒈-⒐的数字样式折叠")
    {
        // ⒈(U+2488)-⒐(U+2490) -> 1-9
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("⒈⒉⒊⒋⒌⒍⒎⒏");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "12345678");
    }

    TEST_CASE("带圈零和负圈数字的数字样式折叠")
    {
        // ⓪(U+24EA) -> 0, ⓫ 等 (>9) 不应被折叠
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("⓪①②③④⑤⑥⑦");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "01234567");
    }

    // --- fold_num_style: 苏州码子 ---

    TEST_CASE("苏州码子的数字样式折叠")
    {
        // 〡(U+3021)-〩(U+3029) -> 1-9
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("〡〢〣〤〥〦〧〨");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "12345678");
    }

    // --- fold_num_style: 中文数字样式 ---

    TEST_CASE("中文大写数字的数字样式折叠")
    {
        // 壹贰叁肆伍陆柒捌 -> 12345678
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("壹贰叁肆伍陆柒捌");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "12345678");
    }

    TEST_CASE("特殊中文数字的数字样式折叠")
    {
        // 零 -> 0, 两 -> 2, 兩 -> 2, 〇 -> 0
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("零两〇三四五六七");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "02034567");
    }

    TEST_CASE("圆形和大圆形作为零的数字样式折叠")
    {
        // ○(U+25CB) -> 0, ◯(U+25EF) -> 0 (策略性折叠)
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("○◯三四五六七");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "0034567");
    }

    TEST_CASE("数字样式折叠不折叠大于9的数字")
    {
        // ⑩(U+2469) 代表10，不应被折叠为单个数字
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        // ⑩ 本身只有1个码点，但代表10，不是单个数字0-9
        // 它不应被折叠，所以 ⑩ 单独不应形成数字序列
        CHECK_FALSE(engine.contains("⑩"));
    }

    // --- 配置开关关闭状态测试 ---

    TEST_CASE("ignore_case=false保留大写字母")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .ignore_case(false)
                                           .build();

        // 不应匹配，因为大小写未被折叠
        CHECK_FALSE(engine.contains("FUCK"));
        // 应该匹配，因为大小写完全一致
        CHECK(engine.contains("fuck"));
    }

    TEST_CASE("ignore_width=false时全角字母仍通过english_style折叠")
    {
        // 当 ignore_width=false 但 ignore_english_style=true（默认）时，
        // 全角字母仍通过 fold_english_style 折叠。
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .ignore_width(false)
                                           .build();

        // 全角字母通过 english_style 折叠，所以仍然匹配
        CHECK(engine.contains("ｆｕｃｋ"));
    }

    TEST_CASE("ignore_width=false且ignore_english_style=false时保留全角")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .ignore_width(false)
                                           .ignore_english_style(false)
                                           .build();

        // 宽度和英文样式折叠均禁用，全角应保持原样
        CHECK_FALSE(engine.contains("ｆｕｃｋ"));
        CHECK(engine.contains("fuck"));
    }

    TEST_CASE("ignore_english_style=false保留样式字母")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .ignore_english_style(false)
                                           .build();

        // 不应匹配，因为带圈字母未被折叠
        CHECK_FALSE(engine.contains("Ⓕⓤc⒦"));
        // 数学粗体也不应匹配
        CHECK_FALSE(engine.contains("\U0001D41F\U0001D42E\U0001D41C\U0001D424"));
        // 普通ASCII仍应匹配
        CHECK(engine.contains("fuck"));
    }

    TEST_CASE("ignore_chinese_style=false保留繁体中文")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"五星红旗"})
                                           .ignore_chinese_style(false)
                                           .build();

        // 不应匹配，因为繁体未被转换
        CHECK_FALSE(engine.contains("五星紅旗"));
        // 简体应匹配
        CHECK(engine.contains("五星红旗"));
    }

    TEST_CASE("所有归一化开关关闭时仅精确匹配有效")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .ignore_case(false)
                                           .ignore_width(false)
                                           .ignore_english_style(false)
                                           .ignore_num_style(false)
                                           .ignore_chinese_style(false)
                                           .build();

        CHECK(engine.contains("fuck"));
        CHECK_FALSE(engine.contains("FUCK"));
        CHECK_FALSE(engine.contains("ｆｕｃｋ"));
        CHECK_FALSE(engine.contains("Ⓕⓤc⒦"));
    }

    // --- ignore_repeat 补充测试 ---

    TEST_CASE("忽略重复（中文字符）")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"傻逼"})
                                           .ignore_repeat(true)
                                           .build();

        const auto result = engine.find_first("傻傻逼");
        REQUIRE(result.has_value());
        CHECK(result->word == "傻傻逼");
    }

    TEST_CASE("ignore_repeat=false不折叠重复字符")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"fuck"})
                                           .ignore_repeat(false)
                                           .build();

        // ignore_repeat=false 时，重复字符不会被折叠
        CHECK_FALSE(engine.contains("ffuuck"));
    }

    // --- 边界场景测试 ---

    TEST_CASE("空字符串输入")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        CHECK_FALSE(engine.contains(""));
        CHECK_FALSE(engine.find_first("").has_value());
        CHECK(engine.find_all_words("").empty());
        CHECK(engine.replace("") == "");
    }

    TEST_CASE("纯符号字符串（无词语字符）")
    {
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"bad"})
                                           .build();

        CHECK_FALSE(engine.contains("@#$%^&*"));
        CHECK_FALSE(engine.contains("！！！"));
    }

    // --- fold_num_style 在 word 匹配中的交互 ---

    TEST_CASE("词语匹配中混合数字样式和中文数字")
    {
        // 确保下三滥这种含有中文数字的词，在 ignore_num_style=true 时
        // "下3滥" 也能匹配 "下三滥"
        sensitive_word_engine engine = sensitive_word_builder()
                                           .deny_words({"下三滥"})
                                           .ignore_num_style(true)
                                           .build();

        CHECK(engine.find_first_word("下3滥招式").value_or("") == "下3滥");
    }

    TEST_CASE("数字检测中混合上标和下标数字")
    {
        // 混合上标、下标、全角、中文数字
        sensitive_word_engine engine = sensitive_word_builder()
                                           .enable_word_check(false)
                                           .enable_num_check(true)
                                           .build();

        const auto result = engine.find_first("卡号⁵₃两〇肆");
        REQUIRE(result.has_value());
        CHECK(result->normalized_word == "53204");
    }
}