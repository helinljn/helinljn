# sensitive-word (C++17)

一个基于 Modern C++ (C++17) 实现的高性能、生产级敏感词检测库。当前聚焦两类核心能力：

- **Word 检测**
- **Number 检测**

项目提供统一的文本归一化、黑白名单管理、动态增删词、结果过滤与替换能力，能够稳健抵御各种复杂的黑产绕过手段（如繁简变体、特殊符号插入、全半角混用、零宽字符穿透等）。适合在本地服务、工具程序或业务系统中进行敏感内容识别与脱敏处理。

---

## 1. 功能概览

当前支持的核心能力：

### Word 检测
- 黑名单敏感词检测
- 白名单优先机制（支持前缀、后缀及夹心重叠对抗）
- `contains`
- `find_first` / `find_first_word`
- `find_all` / `find_all_words`
- `replace`
- 运行期动态增删黑白名单词
- 从文本/文件快速加载 deny / allow 词
- fail-fast（快速失败）与 fail-over（贪婪最大匹配）
- 跨度跳跃机制（智能跳过已匹配词的重叠部分，杜绝冗余误报）

### Number 检测
- 连续数字检测
- `enable_num_check(true)` 开启数字检测
- `num_check_len(n)` 设置最小触发数字长度
- 与主扫描流程深度集成，复用归一化结果
- 返回结果类型 `match_type::num`

### 文本归一化与绕过对抗
- 忽略大小写：`ignore_case`
- 忽略全角半角：`ignore_width`
- 忽略数字样式：`ignore_num_style`（涵盖全角、圈号、上/下标、罗马、苏州码子等数十种变体）
- 忽略中文样式：`ignore_chinese_style`（基于 OpenCC 提供精准的繁转简能力）
- 忽略英文样式：`ignore_english_style`（涵盖数学花体、双线体、哥特体等特殊 Unicode 变体）
- 忽略重复字符：`ignore_repeat`（支持穿透干扰符的尾部重复折叠）
- 无视不可见字符：原生支持穿透零宽空格 (`\u200B`) 等黑产隐藏手段

### 其它能力
- 自定义 `char_ignore`（内置 `special_chars()` 策略）
- 自定义结果条件（内置 `english_word_match()` 边界判定）
- 灵活替换：默认 `*` 替换、指定字符替换、自定义 `replace_strategy`

---

## 2. 架构与性能特点

### 2.1 极致的 Trie 字典树性能
底层放弃了传统的指针链表树或庞大的 Hash 表节点，而是采用了 **Arena Allocator（一维内存池）**：
- 节点分配无内存碎片，对 CPU Cache Locality 极其友好。
- 子节点采用有序 `std::vector` + 二分查找 (`std::lower_bound`)，在空间与时间上取得极佳平衡。
- 根节点配备独立的 `root_ascii_cache_`（O(1) 数组寻址），为海量正常纯英文/数字文本提供极速放行通道。

### 2.2 零拷贝与 Modern C++ 实践
- 接口层大量使用 `std::string_view`，在查询和加载时彻底消除不必要的字符串深拷贝。
- 严格遵循 `[[nodiscard]]`、`constexpr` 与 `noexcept` 语义，接口设计安全严谨。
- 完美的 Pimpl 惯用法，彻底隐藏引擎实现细节，并提供明确的值拷贝/移动语义，方便多实例隔离与并发环境下的实例替换。

### 2.3 单次归一化与高效内存分配
文本在扫描前只做一次归一化。`normalized_text` 直接映射原字符串的精确字节区间（`raw_byte_begin`, `raw_byte_end`），让底层纯净的宽字符（`char32_t`）扫描与上层的 UTF-8 切片替换完美解耦。
对所有可预见的动态字符串拼接（如替换输出、数字提取）均做了 `.reserve()` 预分配，将扩容开销降至最低。

---

## 3. 核心接口与快速示例

### 3.1 构建引擎
使用 `sensitive_word_builder` 创建检测引擎（引擎配置完成后即可通过传值或引用投入使用）：

```cpp
#include "sw/sensitive_word.h"

using sensitive_word::sensitive_word_builder;
using sensitive_word::sensitive_word_engine;
```

### 3.2 基础 Word 检测
```cpp
sensitive_word_engine engine = sensitive_word_builder()
    .deny_words({"五星红旗", "毛主席", "天安门"})
    .build();

engine.contains("五星红旗迎风飘扬");          // true
engine.find_first_word("毛主席的画像");       // "毛主席"
engine.find_all_words("天安门和五星红旗");    // ["天安门", "五星红旗"]
engine.replace("天安门和五星红旗");          // "***和****"
```

### 3.3 Number 检测
```cpp
sensitive_word_engine engine = sensitive_word_builder()
    .enable_word_check(false)
    .enable_num_check(true)
    .num_check_len(8)
    .build();

engine.contains("订单号 12345678");          // true
engine.find_first_word("订单号 12345678");   // "12345678"
```

### 3.4 忽略特殊字符与重叠过滤
```cpp
using sensitive_word::char_ignores::special_chars;

sensitive_word_engine engine = sensitive_word_builder()
    .deny_words({"大傻逼"})
    .char_ignore(special_chars())
    .ignore_repeat(true)
    .build();

// 穿透特殊符号
engine.find_first_word("你这个大傻@逼");       // "大傻@逼"

// 穿透尾部恶意拉长的重复字符以及穿插的干扰符
engine.replace("你个大傻--逼---逼");         // "你个*********"
```

### 3.5 白名单优先机制
```cpp
sensitive_word_engine engine = sensitive_word_builder()
    .deny_words({"龟孙"})
    .allow_words({"龟孙可"})
    .word_fail_fast(false) // 允许贪婪匹配更长的白名单词汇
    .build();

engine.contains("龟孙可");                  // false
```

---

## 4. 关键行为说明

### 4.1 当前实现是 Trie，不是 AC 自动机
本库采用了高度优化的 Trie 字典树配合单次归一化扫描方案：
- 中小规模词库及高并发在线检测场景下表现极佳。
- 运行期动态增删词非常轻量（无需像 AC 自动机那样频繁重建 failure 指针）。
- 在面对单次极长的离线文本流与百万级巨量词库的极限吞吐测试中，速度可能略逊于硬核 AC 自动机，但在业务灵活性上具有绝对优势。

### 4.2 fail-fast / fail-over (最大贪婪匹配)
- `word_fail_fast(true)`（默认）：在同一起点一旦命中完整的敏感词即刻停止向后探索。
- `word_fail_fast(false)`：在同一起点会继续沿着树向下探索，以找到**最长的命中词**（例如同时存在 `中国` 和 `中国人`，输入 `我是中国人` 时将只返回 `中国人`）。
- **跨度跳跃**：无论是哪种模式，一旦确认了一个最终命中的敏感词，主扫描游标会直接跳过该词覆盖的跨度，**不会发生交叉重叠的重复输出**。

### 4.3 英文单词边界判定
引擎默认启用了 `english_word_match()` 结果过滤策略。
如果匹配到的纯英文敏感词（如 `av`），在其原始文本左右两边仍然紧连着英文字母（如 `have`），该命中会被视为误杀并安全放行。如果你需要检测子串，可使用 `.result_condition(always_true())` 覆盖默认行为。

---

## 5. 归一化深度说明

### 5.1 繁简体与 OpenCC
基于 OpenCC 实现精准的繁简转化（默认 `t2s` 方向）。我们采用了“整词/整段优先转换”的策略，只有在必要时才回退到逐字处理，极大地减少了热路径上的 FFI 转换开销。

### 5.2 数字样式支持 (ignore_num_style)
涵盖了极广的 Unicode 数字变体：
- 圈号/带括号数字：①, ⑴, ⒈
- 数学字体数字：粗体 𝟏、双线体 𝟙、无衬线 𝟣
- 上下标：⁵, ₃
- 中文大写与特殊数字：壹, 两, 〇, 〡 (苏州码子)

### 5.3 英文变体 (ignore_english_style)
不仅处理全/半角及大小写，还支持折叠诸如花体 `ℬ`、哥特体 `𝔅`、双线体 `𝔹` 等类字母符号，有效打击黑产的形近字替换攻击。

---

## 6. 构建与测试

### 6.1 构建
Windows 下按以下方式构建：

```bat
build.windows.bat release
```
*(可使用 `debug` 构建调试版本)*

### 6.2 运行测试
本项目附带了极其详尽的 Doctest 单元测试（涵盖基础流程、归一化、高级边界碰撞、零宽字符穿透等）。
运行测试时，请在构建目录下执行：

```bat
cd .build/release && test.exe
```

---

## 7. 资源说明

`res/` 目录下存放敏感词相关资源文件（如 OpenCC 词典或基础黑白名单库）。具体配置可由业务调用方通过 `.add_deny_words_from_file(...)` 灵活加载。

---

## 8. 一句话总结

这是一个专注于 **Word + Number** 拦截的现代 C++ (C++17) 敏感词检测库，强调：
**极致的内存性能、高维度的绕过对抗、线程安全的动态增删、以及符合直觉的 API 设计。**
