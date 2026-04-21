# sensitive-word (C++17)

一个基于 C++17 实现的敏感词检测库，当前聚焦两类核心能力：

- **Word 检测**
- **Number 检测**

项目提供统一的文本归一化、黑白名单管理、动态增删词、结果过滤与替换能力，适合在本地服务、工具程序或业务系统中进行敏感内容识别与脱敏处理。

---

## 1. 功能概览

当前支持的核心能力：

### Word 检测
- 黑名单敏感词检测
- 白名单优先机制
- `contains`
- `find_first`
- `find_all`
- `replace`
- 动态新增黑名单词
- 动态删除黑名单词
- 动态新增白名单词
- 动态删除白名单词
- 从文本加载 deny / allow 词
- 从文件加载 deny / allow 词
- fail-fast
- fail-over

### Number 检测
- 连续数字检测
- `enable_num_check(true)` 开启数字检测
- `num_check_len(n)` 设置最小数字长度
- 与主扫描流程集成
- 返回结果类型 `match_type::num`

### 文本归一化
- 忽略大小写：`ignore_case`
- 忽略全角半角：`ignore_width`
- 忽略数字样式：`ignore_num_style`
- 忽略中文样式：`ignore_chinese_style`
- 忽略英文样式：`ignore_english_style`
- 忽略重复字符：`ignore_repeat`

### 其它能力
- 自定义 `char_ignore`
- 内置 `special_chars()`
- 自定义结果条件
- 默认 `*` 替换
- 指定字符替换
- 自定义 `replace_strategy`

---

## 2. 项目特点

### 2.1 Word + Number 双主链路
项目围绕两类高频检测任务构建：

- **Word**：适合敏感词、违禁词、黑名单词检测
- **Number**：适合长数字串、号码类文本识别

### 2.2 单次归一化
文本在扫描前只做一次归一化，避免在每个检测步骤中重复处理字符。

### 2.3 支持动态增删词
运行期可以直接：

- `add_word`
- `remove_word`
- `add_allow_word`
- `remove_allow_word`

适合规则需要热更新的场景。

### 2.4 白名单优先
当白名单与黑名单发生冲突时，优先按白名单处理。

---

## 3. 核心接口

### 3.1 构建引擎
使用 `sensitive_word_builder` 创建检测引擎：

```cpp
#include "sensitive_word.h"

using sensitive_word::sensitive_word_builder;
using sensitive_word::sensitive_word_engine;
```

### 3.2 常用能力
```cpp
sensitive_word_engine engine = sensitive_word_builder()
    .deny_words({"五星红旗", "毛主席", "天安门"})
    .build();

bool has_sensitive = engine.contains("五星红旗迎风飘扬");
auto first = engine.find_first_word("五星红旗迎风飘扬");
auto all = engine.find_all_words("五星红旗迎风飘扬，毛主席万岁");
auto replaced = engine.replace("五星红旗迎风飘扬");
```

---

## 4. 快速示例

## 4.1 基础 Word 检测
```cpp
sensitive_word_engine engine = sensitive_word_builder()
    .deny_words({"五星红旗", "毛主席", "天安门"})
    .build();

engine.contains("五星红旗迎风飘扬");          // true
engine.find_first_word("毛主席的画像");       // "毛主席"
engine.find_all_words("天安门和五星红旗");    // ["天安门", "五星红旗"]
engine.replace("天安门和五星红旗");          // "***和****"
```

## 4.2 Number 检测
```cpp
sensitive_word_engine engine = sensitive_word_builder()
    .enable_word_check(false)
    .enable_num_check(true)
    .num_check_len(8)
    .build();

engine.contains("订单号 12345678");          // true
engine.find_first_word("订单号 12345678");   // "12345678"
```

## 4.3 忽略特殊字符
```cpp
using sensitive_word::char_ignores::special_chars;

sensitive_word_engine engine = sensitive_word_builder()
    .deny_words({"傻冒"})
    .char_ignore(special_chars())
    .build();

engine.find_first_word("你这个傻@冒");       // "傻@冒"
engine.replace("你这个傻@冒");              // "你这个***"
```

## 4.4 白名单优先
```cpp
sensitive_word_engine engine = sensitive_word_builder()
    .deny_words({"龟孙"})
    .allow_words({"龟孙可"})
    .word_fail_fast(false)
    .build();

engine.contains("龟孙可");                  // false
```

---

## 5. 关键行为说明

## 5.1 当前实现是 Trie，不是 AC 自动机
当前 Word 主链路采用：

- Trie 字典树
- 逐起点扫描
- 单次文本归一化
- 白名单 / 黑名单并行判断

这意味着：

- 中小规模场景通常足够使用
- 在大词库、长文本、高吞吐场景下，性能更容易出现瓶颈
- 当前实现更偏向：
  - 语义清晰
  - 动态更新方便
  - 行为可控

---

## 5.2 `ignore_num_style` 会影响 Word 和 Number
`ignore_num_style` 不仅影响 Number 检测，也会影响 Word 检测中的词归一化。

例如：

- 词库：`下三滥`
- 文本：`下3滥`

当 `ignore_num_style(true)` 时，两者可以匹配。
当 `ignore_num_style(false)` 时，不会跨样式命中。

---

## 5.3 `char_ignore` 会影响原始跨度
启用忽略字符策略后：

- 匹配时可以跳过某些字符
- 但最终返回的原始区间、替换长度，可能覆盖这些被忽略字符

因此：

- `find_first().word`
- `replace()`

的结果长度，有时会比“纯净命中词”更长。

---

## 5.4 fail-fast / fail-over
当前已支持两种扫描模式：

- `word_fail_fast(true)`：快速失败模式
- `word_fail_fast(false)`：尽量继续匹配更长结果

适合在“优先速度”和“优先最长命中”之间做选择。

---

## 6. 归一化说明

## 6.1 繁简体支持
当前版本支持基础繁简体归一化。

实现方式：

- 使用 `ignore_chinese_style`
- 在 `text_normalizer` 中接入 OpenCC
- 当前采用 `t2s` 方向进行归一化（繁体转简体）

例如：

- 词库：`五星红旗`
- 输入：`五星紅旗`

默认配置下可以正常命中。

## 6.2 数字样式支持
当前已覆盖一批常见数字变体，包括但不限于：

- 全角数字
- 圈号数字
- 上标数字
- 下标数字
- 部分括号数字 / 带点数字
- 常见中文数字
- 常见中文大写数字

说明：

- 当前实现已经覆盖当前测试所需范围
- 并不意味着所有 Unicode 数字变体都已完全穷尽

---

## 7. 公开配置边界

当前 Builder 主要用于以下方向：

- Word 相关
- Number 相关
- 归一化相关
- 白名单 / 黑名单
- 结果条件
- 替换策略
- 忽略字符

当前版本不包含以下方向的配置接口：

- Email 检测
- URL 检测
- IPv4 检测

---

## 8. 构建与测试

## 8.1 构建
Windows 下按以下方式构建：

```bat
build.windows.bat debug
```

## 8.2 运行测试
运行测试时，请在 `.build/Debug` 目录执行：

```bat
cd .build/Debug && test.exe
```

---

## 9. 资源说明

`resources/` 目录下存放敏感词相关资源文件。

README 只说明项目能力与使用方式，不展开资源文件内容说明。

---

## 10. 当前状态

当前版本已经具备以下基础能力：

- Word 检测主链路
- Number 检测主链路
- Trie 黑白名单
- 归一化基础能力
- 基于 OpenCC 的基础繁简体支持
- 结果条件基础能力
- 替换策略基础能力
- 动态增删词
- 文本 / 文件词加载
- 一批核心行为测试覆盖

---

## 11. 一句话总结

这是一个专注于 **Word + Number** 的 C++17 敏感词检测库，强调：

- 统一归一化
- 黑白名单控制
- 动态增删词
- 可解释、可测试、可扩展的行为语义
