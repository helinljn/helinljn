# sensitive-word (C++17)

一个基于 Modern C++ (C++17) 实现的高性能、生产级敏感词检测库。当前聚焦两类核心能力：

- **Word 检测**
- **Number 检测**

项目提供统一的文本归一化、黑白名单管理、结果过滤与替换能力，能够稳健抵御各种复杂的黑产绕过手段（如繁简变体、特殊符号插入、全半角混用、零宽字符穿透等）。其中符号插入和零宽字符穿透需要启用内置字符忽略策略 `char_ignores::special_chars()`。适合在本地服务、工具程序或业务系统中进行敏感内容识别与脱敏处理。

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
- 从文本/文件快速加载 deny / allow 词
- 匹配策略分离：支持执行时配置最短匹配（快速失败）与最长匹配（贪婪匹配）
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
- 忽略中文样式：`ignore_chinese_style`（基于单字映射缓存提供极速的繁转简能力）
- 忽略英文样式：`ignore_english_style`（涵盖数学花体、双线体、哥特体等特殊 Unicode 变体）
- 忽略重复字符：`ignore_repeat`（支持穿透干扰符的尾部重复折叠）
- 字符跳过策略：配合 `char_ignores::special_chars()` 可穿透特殊符号、标点和零宽空格 (`\u200B`) 等隐藏字符

### 其它能力
- 自定义 `char_ignore`（默认 `none()` 不跳过字符，内置 `special_chars()` 用于跳过非词字符）
- 自定义结果条件（内置 `english_word_match()` 边界判定）
- 灵活替换：默认 `*` 替换、指定字符替换、自定义 `replace_strategy`

---

## 2. 架构与性能特点

### 2.1 极速的单字符繁简映射缓存
我们彻底摒弃了常规做法中将整段文本丢给 OpenCC 进行词法分词与长句繁转简的高昂开销。
通过在归一化器实例中维护不到 100KB 的连续数组缓存，我们将最常用的两万个基础 CJK 汉字映射进行了快速缓存（`O(1)` 数组级访问）。扩展区汉字则使用映射缓存。该机制避免了每次扫描都对整段文本做 OpenCC 分词式转换，并能在启用字符忽略策略时处理 `敏*感*詞` 这类逐字穿插干扰符的输入。

### 2.2 极致的 Trie 字典树性能
底层放弃了传统的指针链表树或庞大的 Hash 表节点，而是采用了 **Arena Allocator（一维内存池）**：
- 节点分配无内存碎片，对 CPU Cache Locality 极其友好。
- 子节点采用有序 `std::vector` + 二分查找 (`std::lower_bound`)，在空间与时间上取得极佳平衡。
- 根节点配备独立的 `root_ascii_cache_`（O(1) 数组寻址），为海量正常纯英文/数字文本提供极速放行通道。

### 2.3 零拷贝与 Modern C++ 实践
- 接口层大量使用 `std::string_view`，在查询和加载时彻底消除不必要的字符串深拷贝。
- 严格遵循 `[[nodiscard]]`、`constexpr` 与 `noexcept` 语义，接口设计安全严谨。
- 完美的 Pimpl 惯用法，彻底隐藏引擎实现细节，并提供明确的值拷贝/移动语义，方便多实例隔离与并发环境下的实例替换。

### 2.4 单次归一化与无碎片内存分配
文本在扫描前只做一次归一化。我们将 UTF-8 解码、全半角折叠、繁简映射与原始字节区间的对齐工作在**单次遍历循环中同步完成**，彻底消除了中间临时数组的创建和拷贝开销。
对所有可预见的动态字符串拼接（如替换输出、数字提取）均做了 `.reserve()` 预分配，结合 `normalized_text` 对原字符串精确字节区间（`raw_byte_begin`, `raw_byte_end`）的映射，让底层纯净的宽字符（`char32_t`）扫描与上层切片替换完美解耦，将堆内存分配与回收的开销降至极限。

---

## 3. 核心接口与快速示例

### 3.1 构建引擎
使用 `sensitive_word_builder` 创建检测引擎（引擎配置完成后即可通过传值或引用投入使用）。业务目标如果把 `src/core` 加入 include path，可直接包含 `sw/sensitive_word.h`；如果把 `src` 加入 include path，可包含仓库内的汇总头 `core/sw.h`：

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
engine.find_first_word("毛主席的画像").value_or(""); // "毛主席"
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
engine.find_first_word("订单号 12345678").value_or(""); // "12345678"
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
engine.find_first_word("你这个大傻@逼").value_or(""); // "大傻@逼"

// 穿透尾部恶意拉长的重复字符以及穿插的干扰符
engine.replace("你个大傻--逼---逼");         // "你个*********"
```

### 3.5 运行时匹配策略与白名单优先机制
```cpp
sensitive_word_engine engine = sensitive_word_builder()
    .deny_words({"龟孙", "敏感词", "大敏感词"})
    .allow_words({"龟孙可"})
    .build();

// contains 内部强锁最短匹配，性能极高
engine.contains("龟孙可");                  // false

// 查询/替换支持执行时传入 match_options 控制策略
sensitive_word::match_options longest_opts{true};
engine.find_first_word("大敏感词", longest_opts).value_or(""); // 贪婪匹配更长的词，返回 "大敏感词"
```

---

## 4. 关键行为说明

### 4.1 当前实现是 Trie，不是 AC 自动机
本库采用了高度优化的 Trie 字典树配合单次归一化扫描方案：
- 中小规模词库及高并发在线检测场景下表现极佳。
- 运行期动态增删词非常轻量（无需像 AC 自动机那样频繁重建 failure 指针）。
- 在面对单次极长的离线文本流与百万级巨量词库的极限吞吐测试中，速度可能略逊于硬核 AC 自动机，但在业务灵活性上具有绝对优势。

### 4.2 最短匹配 (shortest) 与最长匹配 (longest)
通过提取搜索策略为运行期选项（`match_options`），单一引擎实例即可高效兼顾“判有无”与“准替换”：
- `longest_match = false`（默认）：最短匹配（快速失败）。在同一起点一旦命中完整的敏感词即刻停止向后探索（性能最高）。`contains` 方法内部固定采用此模式。
- `longest_match = true`：最长匹配（贪婪匹配）。在同一起点会继续沿着树向下探索，以找到**最长的命中词**（例如同时存在 `中国` 和 `中国人`，输入 `我是中国人` 时将只返回 `中国人`），在进行高亮或替换时通常需要开启此选项。
- **跨度跳跃**：无论是哪种模式，一旦确认了一个最终命中的敏感词，主扫描游标会直接跳过该词覆盖的跨度，**不会发生交叉重叠的重复输出**。

### 4.3 英文单词边界判定
引擎默认启用了 `english_word_match()` 结果过滤策略。
如果匹配到的纯英文敏感词（如 `av`），在其原始文本左右两边仍然紧连着英文字母（如 `have`），该命中会被视为误杀并安全放行。如果你需要检测子串，可使用 `.result_condition(always_true())` 覆盖默认行为。

---

## 5. 归一化深度说明

### 5.1 繁简体单字映射缓存
归一化器通过连续数组构建基础 CJK 汉字的繁简映射缓存，实现 `O(1)` 的单字级访问；基础区以外的兼容或扩展区汉字使用 `unordered_map` 缓存。实现上只对单个代码点做 OpenCC 转换和缓存，不把整段文本交给第三方分词流程处理。启用字符忽略策略后，敏感词中间穿插干扰符号（如 `敏*感*詞`）也能按逐字折叠后的结果继续匹配。

### 5.2 数字样式支持 (ignore_num_style)
涵盖了极广的 Unicode 数字变体：
- 圈号/带括号数字：①, ⑴, ⒈
- 数学字体数字：粗体 𝟏、双线体 𝟙、无衬线 𝟣
- 上下标：⁵, ₃
- 中文大写与特殊数字：壹, 两, 〇, 〡 (苏州码子)

### 5.3 英文变体 (ignore_english_style)
不仅处理全/半角及大小写，还支持折叠诸如花体 `ℬ`、哥特体 `𝔅`、双线体 `𝔹` 等类字母符号，有效打击黑产的形近字替换攻击。

---

## 6. 性能基准测试 (Benchmark)

使用 `nanobench` 结合**海量真实本地词库**，对 100 条混合长文本（长度在 15~150 字不等，50% 为正常文本，50% 包含各类脏话变体）进行单线程基准测试。

我们在 Windows 原生环境与 WSL (Linux) 环境下分别进行了压测，**WSL (Linux) 环境下的性能表现甚至更胜一筹**（得益于 GCC/Clang 更激进的循环展开优化，以及 Linux 下 mimalloc / mmap 更高效的虚拟内存管理）：

### 极速优化测试结果对比 (Windows vs WSL Linux)：

**Windows 环境测试结果：**
|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|          223,992.45 |            4,464.44 |    0.3% |      0.13 | `sw: find_first (100 mixed strings)`
|          370,768.42 |            2,697.10 |    0.7% |      0.22 | `sw: find_all (100 mixed strings)`
|          389,772.00 |            2,565.60 |    0.6% |      0.23 | `sw: replace (100 mixed strings)`

**WSL (Linux) 环境测试结果：**
|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|          154,001.89 |            6,493.43 |    0.9% |      0.09 | `sw: find_first (100 mixed strings)`
|          253,812.28 |            3,939.92 |    0.9% |      0.15 | `sw: find_all (100 mixed strings)`
|          271,183.02 |            3,687.55 |    0.6% |      0.16 | `sw: replace (100 mixed strings)`

**核心指标深度解读：**
1. **百万级别吞吐，微秒级响应**：`1 op` 代表处理完完整的 **100** 条长短混合文本。按上表数据折算，Linux 环境下 `find_first` 单条文本平均约 **1.5 微秒**，单核吞吐量 Windows 约 **44 万条/秒**，WSL Linux 约 **64 万条/秒**；全文标记提取或替换也能达到单核 **36 万+ 条/秒**。
2. **提前阻断效率高**：无论在哪个平台，对比 `find_all`，`find_first` 耗时均缩短了 35% ~ 45%。这说明在只需要判断是否命中的场景下，短路退出机制能明显降低扫描成本。
3. **脱敏替换额外开销低**：从交叉对比来看，`replace`（文本脱敏重组）的耗时与单纯的 `find_all` 提取结果之间只有不到 6% 的差距，主要得益于基于原始字节区间的切片拼接和 `.reserve()` 预分配。
4. **平台差异明显**：这组测试中 WSL Linux 的速度比 Windows 环境快了近 45%，可能与编译器优化、运行时库和内存分配实现差异有关。不同硬件、编译器和词库规模下，实际结果需要以本地 benchmark 为准。
5. **硬件亲和性与 L3 缓存敏感**：作为典型的内存密集型 (Memory Bound) 任务，引擎底层的 Trie 树搜索会受到 CPU **L3 缓存大小**和内存频率影响。Arena 一维内存池可以减少节点碎片，有利于提升缓存局部性。

---

## 7. 构建与测试

### 7.0 环境前提

- Windows：当前脚本使用 Visual Studio 17 2022 的 x64 生成器。
- Linux/WSL：需要 CMake、支持 C++17 的 GCC/Clang、Python3。
- 构建 OpenCC 数据目标需要 Python3；首次构建会在运行目录生成 `data/config` 与 `data/dictionary`，供繁简转换使用。

### 7.1 准备第三方依赖
首次构建或 `3rd/` 依赖缺失时，先初始化第三方依赖：

```bat
init-3rd.bat
```

Linux/WSL 下使用：

```bash
./init-3rd.sh
```

### 7.2 构建
Windows 下按以下方式构建：

```bat
build.windows.bat release
```
*(可使用 `debug` 构建调试版本，或使用 `all` 同时构建 Debug / Release)*

当前敏感词能力随 `core` 动态库构建并导出。业务侧使用时包含 `sw/sensitive_word.h`，并链接 `core`；测试与基准目标也通过链接 `core` 使用同一套实现。Release 构建后的主要产物位于 `.build\Release`，包括 `core.dll`、`test.exe`、`benchmark.exe` 和 hcode 相关实验目标。

Linux/WSL 下按以下方式构建：

```bash
./build.linux.sh release
```

Release 构建产物同样输出到 `.build/Release`。

运行程序时建议从 `.build\Release` 或 `.build\Debug` 目录启动，或把构建生成的 `data/config`、`data/dictionary` 放到进程当前工作目录下。默认启用 `ignore_chinese_style` 时，繁转简会从当前工作目录的 `./data/config` 与 `./data/dictionary` 查找 OpenCC 资源；资源缺失时该能力会降级为不转换。

### 7.3 运行测试
本项目附带了极其详尽的 Doctest 单元测试（涵盖基础流程、归一化、高级边界碰撞、零宽字符穿透等）。
运行测试时，请在构建目录下执行：

```bat
cd .build\Release
.\test.exe
```

### 7.4 运行基准测试

```bat
cd .build\Release
.\benchmark.exe
```

---

## 8. 资源说明

`res/` 目录下存放敏感词相关资源文件，当前包含优化词库样例 `dict-2026-04-20.txt`。具体配置可由业务调用方通过 `.add_deny_words_from_file(...)` 灵活加载。

词库加载器按行读取词条，会自动裁剪每行首尾空白；空行、`#` 开头的行和 `//` 开头的行会被跳过。加载失败时 `.add_*_words_from_file(...)` 会抛出运行时异常。

构建时会把 `res/` 复制到运行目录下，因此测试和基准可以直接从 `.build\Release\res\dict-2026-04-20.txt` 读取样例词库。OpenCC 配置与二进制词典不放在 `res/` 中，构建时会生成或复制到运行目录下的 `data/config` 与 `data/dictionary`。

`scripts/optimize_dict.py` 可用于词库清洗、去重、长度过滤和报告输出。运行脚本需要 Python 依赖 `charset-normalizer` 与 `opencc-python-reimplemented`。

---

## 9. 第三方依赖概览

`init-3rd.bat` / `init-3rd.sh` 会拉取以下依赖；其中 brynet、distorm、funchook 当前跟随远端 HEAD，其余依赖按脚本中的 tag 固定：

| 依赖 | 版本 | 用途 |
|------|------|------|
| `brynet` | HEAD | 异步网络库，供 `core` 暴露头文件和网络相关测试使用 |
| `distorm` | HEAD | 反汇编库，供 `funchook` 使用 |
| `funchook` | HEAD | Hook/热补丁实验相关能力，`core` 当前链接该库 |
| `utfcpp` | v4.0.9 | UTF-8/16/32 处理 |
| `opencc` | ver.1.2.0 | 繁简转换与 OpenCC 数据生成 |
| `spdlog` | v1.15.3 | 日志 |
| `jsoncpp` | 1.9.7 | JSON 解析 |
| `doctest` | v2.4.12 | 测试框架 |
| `mimalloc` | v2.3.2 | 内存分配器，供 test/benchmark/hcode 链接 |
| `nanobench` | v4.3.11 | 微基准测试 |

---

## 10. 一句话总结

这是一个专注于 **Word + Number** 拦截的现代 C++ (C++17) 敏感词检测库，强调：
**高性能内存布局、高维度绕过对抗、运行时动态增删词、以及符合直觉的 API 设计。**

同一引擎实例支持运行时动态增删词；如果存在并发读写，调用方需要自行同步，或通过构建新实例并原子替换引用的方式发布词库更新。
