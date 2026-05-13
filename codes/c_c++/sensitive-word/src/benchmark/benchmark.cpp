#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"
#include "mimalloc.h"
#include "core/sw.h"
#include <cstdlib>
#include <string>
#include <vector>

namespace {

/**
 * @brief 验证 mimalloc 库是否被正确加载
 * @return true 如果 mimalloc 库被正确加载，否则返回 false
 */
bool verify_mimalloc()
{
    // 1. 验证 mimalloc 库版本
    if (mi_version() != MI_MALLOC_VERSION)
        return false;

    // 2. 验证 std::malloc 是否被 mimalloc 接管
    void* p1 = std::malloc(1024);
    if (p1 == nullptr)
        return false;

    const bool malloc_from_mimalloc = mi_is_in_heap_region(p1);
    std::free(p1);

    if (!malloc_from_mimalloc)
        return false;

    // 3. 验证全局 new/delete 是否被 mimalloc 接管
    void* p2 = static_cast<void*>(new char[1024]);

    const bool new_from_mimalloc = mi_is_in_heap_region(p2);
    delete[] static_cast<char*>(p2);

    if (!new_from_mimalloc)
        return false;

    return true;
}

} // namespace

void run_sensitive_word_benchmark()
{
    using namespace sensitive_word;

    sensitive_word_engine engine = sensitive_word_builder()
                                       .add_deny_words_from_file("./res/dict-2026-04-20.txt")
                                       .build();

    std::vector<std::string> clean_templates = {
        "今天天气真不错，适合出去玩。", "我们要好好学习，天天向上。",
        "这家餐厅的菜味道很好，推荐给大家。", "早上好，今天的工作计划是什么？",
        "看了一部非常好看的电影。", "你好，请问去火车站怎么走？",
        "周末打算和朋友去爬山。", "这件衣服看起来很合身，不错。",
        "项目已经按照计划顺利完成了。", "感谢您的帮助，祝您生活愉快。"
    };

    std::vector<std::string> dirty_words = {
        "傻逼", "操你妈", "他妈的", "制服丝袜", "煞笔",
        "草泥马", "王八蛋", "神经病", "找小姐", "贱人"
    };

    std::vector<std::string> test_strings;
    test_strings.reserve(100);

    // 50 句无脏话文本
    for (int i = 0; i < 50; ++i)
    {
        std::string s;

        int parts = (i % 5) + 1;
        for (int j = 0; j < parts; ++j)
        {
            s += clean_templates[(i + j) % clean_templates.size()];
        }

        test_strings.push_back(std::move(s));
    }

    // 50 句带脏话文本
    for (int i = 0; i < 50; ++i)
    {
        std::string s;

        int parts = (i % 4) + 1;
        for (int j = 0; j < parts; ++j)
        {
            s += clean_templates[(i + j) % clean_templates.size()];
        }

        // 在中间或末尾插入脏话
        s += dirty_words[i % dirty_words.size()];
        if (i % 2 == 0)
            s += clean_templates[(i + 1) % clean_templates.size()];

        test_strings.push_back(std::move(s));
    }

    // 运行 benchmark
    ankerl::nanobench::Bench().minEpochIterations(50).run("sw: find_first (100 mixed strings)", [&] {
        for (const auto& str : test_strings)
        {
            ankerl::nanobench::doNotOptimizeAway(engine.find_first(str));
        }
    });

    ankerl::nanobench::Bench().minEpochIterations(50).run("sw: find_all (100 mixed strings)", [&] {
        for (const auto& str : test_strings)
        {
            ankerl::nanobench::doNotOptimizeAway(engine.find_all(str));
        }
    });

    ankerl::nanobench::Bench().minEpochIterations(50).run("sw: replace (100 mixed strings)", [&] {
        for (const auto& str : test_strings)
        {
            ankerl::nanobench::doNotOptimizeAway(engine.replace(str));
        }
    });
}

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    std::ignore = argc;
    std::ignore = argv;

    if (!verify_mimalloc())
        return EXIT_FAILURE;

    run_sensitive_word_benchmark();

    return EXIT_SUCCESS;
}
