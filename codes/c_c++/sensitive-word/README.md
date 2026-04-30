# sensitive-word (C++17)

一个基于 Modern C++ (C++17) 实现的高性能敏感词检测源码模块，并提供本地 HTTP 服务目标。当前聚焦两类核心能力：

- **Word 检测**
- **Number 检测**

项目提供统一的文本归一化、黑白名单管理、结果过滤、替换和本地 HTTP 服务能力，能够抵御多类复杂文本绕过手段（如繁简变体、特殊符号插入、全半角混用，以及配置特殊字符忽略后的零宽字符穿透等）。适合在本地服务、工具程序或业务系统中进行敏感内容识别与脱敏处理。

当前 CMake 工程没有单独产出 `sensitive-word` 库目标；`src/service/sw` 下的敏感词模块源码会被 `service`、`test`、`benchmark` 等目标直接编入。若需要作为独立库分发，建议先在 CMake 中补充独立的静态库、动态库或对象库目标。

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
- 配置 `char_ignores::special_chars()` 后，可穿透零宽空格 (`\u200B`) 等不可见字符和常见干扰符

### 其它能力
- 自定义 `char_ignore`（内置 `special_chars()` 策略）
- 自定义结果条件（内置 `english_word_match()` 边界判定）
- 灵活替换：默认 `*` 替换、指定字符替换、自定义 `replace_strategy`
- 本地 HTTP 服务：提供 `/v1/check`、`/v1/query_word`、`/v1/healthz`

---

## 2. 架构与性能特点

### 2.1 极速的单字符繁简映射缓存
我们彻底摒弃了常规做法中将整段文本丢给 OpenCC 进行词法分词与长句繁转简的高昂开销。
通过在内存中静态分配不到 100KB 的连续数组，我们将最常用的两万个基础 CJK 汉字映射进行了极速缓存（`O(1)` 数组级访问）。不仅将繁简预处理的耗时降至微秒级，更在核心机制上确保了即使敏感词中间被穿插了各类干扰符号（如 `敏*感*詞`），依然能被**逐字精准折叠拦截**，大幅提升了防黑产穿透的健壮性。

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
engine.find_first_word("大敏感词", longest_opts); // 贪婪匹配更长的词，返回 "大敏感词"
```

### 3.6 HTTP 服务接口

项目包含一个基于 brynet 的本地 HTTP 服务目标 `service.exe`。默认配置监听 `0.0.0.0:9000`，从以下文件加载词库：

- `res/sensitive_word_deny.txt`
- `res/sensitive_word_allow.txt`

启动方式：

```powershell
.\.build\Release\service.exe
```

通用响应格式：

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "optional-request-id",
  "data": {}
}
```

错误响应同样使用 `code`、`message`、`data`，并在请求中提供 `request_id` 时原样返回。`/v1/check` 与 `/v1/query_word` 需要 `POST` 和 `Content-Type: application/json`；`/v1/healthz` 支持 `GET` 与 `POST`。

#### `/v1/healthz`

健康检查接口：

```powershell
curl.exe http://127.0.0.1:9000/v1/healthz
```

返回示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {
    "status": "up",
    "worker_count": 8
  }
}
```

#### `/v1/check`

文本检测接口。请求字段：

- `text`：必填，待检测文本。
- `request_id`：可选，请求链路标识。
- `return_mode`：可选，`contains` / `first` / `all`，默认 `all`。
- `longest_match`：可选，是否启用最长匹配，默认 `false`。
- `max_results`：可选，`all` 模式下最多返回的结果数，默认 `100`。

请求示例：

```powershell
curl.exe -X POST http://127.0.0.1:9000/v1/check `
  -H "Content-Type: application/json" `
  -d "{\"request_id\":\"demo-1\",\"text\":\"天安门和五星红旗\",\"return_mode\":\"all\",\"longest_match\":true}"
```

返回示例：

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "demo-1",
  "data": {
    "hit": true,
    "match_count": 2,
    "matches": [
      {
        "word": "天安门",
        "normalized_word": "天安门",
        "type": "word",
        "raw_begin": 0,
        "raw_end": 9,
        "raw_code_point_length": 3
      },
      {
        "word": "五星红旗",
        "normalized_word": "五星红旗",
        "type": "word",
        "raw_begin": 12,
        "raw_end": 24,
        "raw_code_point_length": 4
      }
    ]
  }
}
```

`return_mode` 行为：

- `contains`：只返回是否命中，适合高吞吐网关场景。
- `first`：返回第一个命中结果。
- `all`：返回全部命中结果，并受 `max_results` 限制。

#### `/v1/query_word`

查询词条在当前 worker 引擎中的状态：

```powershell
curl.exe -X POST http://127.0.0.1:9000/v1/query_word `
  -H "Content-Type: application/json" `
  -d "{\"request_id\":\"demo-2\",\"word\":\"天安门\"}"
```

返回示例：

```json
{
  "code": 0,
  "message": "ok",
  "request_id": "demo-2",
  "data": {
    "word": "天安门",
    "exists": true,
    "in_deny": true,
    "in_allow": false
  }
}
```

---

## 4. 关键行为说明

### 4.1 当前实现是 Trie，不是 AC 自动机
本库采用了高度优化的 Trie 字典树配合单次归一化扫描方案：
- 中小规模词库及高并发在线检测场景下表现极佳。
- 单实例运行期增删词非常轻量（无需像 AC 自动机那样频繁重建 failure 指针）。
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
我们通过静态分配连续数组，构建了基础 CJK 汉字的繁简映射缓存，实现 `O(1)` 的单字级极速访问。彻底摒弃了传统做法中将整段文本交由第三方分词库处理的高昂开销。这种设计不仅将热路径耗时降至极限，更确保了即使敏感词中间穿插了各类干扰符号（如 `敏*感*詞`），依然能被**逐字精准折叠拦截**，大幅提升了引擎防黑产穿透的健壮性。

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

以下数据来自一次历史测试结果，用于说明当前实现的大致性能量级。复现时建议记录 CPU、编译器版本、构建类型、词库版本和运行命令；不同硬件、编译器和系统内存分配器会显著影响结果。

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

**核心指标解读：**
1. `1 op` 代表处理完完整的 **100** 条长短混合文本。按这组历史数据折算，`find_first` 在单线程下可以达到微秒级单条处理延迟。
2. 对比 `find_all`，`find_first` 耗时更低，符合“只判断是否命中”场景的短路退出预期。
3. `replace` 的耗时接近 `find_all`，说明当前替换路径的切片拼接和 `.reserve()` 预分配能有效控制额外分配开销。
4. Windows 与 Linux/WSL 的结果差异可能来自编译器优化、运行库和内存分配器差异，不能直接外推到所有部署环境。
5. Trie 搜索对 CPU 缓存和内存访问模式较敏感，词库规模、文本分布和硬件缓存都会影响最终吞吐。

---

## 7. 构建与测试

### 7.1 构建
首次构建或第三方依赖缺失时，先初始化 `3rd/` 依赖：

```powershell
.\init-3rd.bat
```

Windows 下按以下方式构建：

```powershell
.\build.windows.bat release
```
*(可使用 `debug` 构建调试版本)*

构建 OpenCC 数据目标需要可用的 Python3；项目会在构建运行目录下生成 `data/config` 与 `data/dictionary`。

### 7.2 运行测试
本项目附带了较完整的 Doctest 单元测试（涵盖基础流程、归一化、高级边界碰撞、零宽字符穿透等）。
运行测试时，请在构建目录下执行：

```powershell
cd .build\Release
.\test.exe
```

---

## 8. 资源说明

`res/` 目录下存放项目敏感词相关资源文件，例如基础黑白名单和大词库样例。具体配置可由业务调用方通过 `.add_deny_words_from_file(...)` 灵活加载。

OpenCC 配置和词典不直接存放在 `res/`；构建时会从 `3rd/opencc/data` 生成或复制到运行目录下的 `data/config` 与 `data/dictionary`。运行依赖繁简转换能力时，需要确保程序工作目录能访问这两个目录。

---

## 9. 一句话总结

这是一个专注于 **Word + Number** 拦截的现代 C++ (C++17) 敏感词检测源码模块，强调：
**极致的内存性能、高维度的绕过对抗、清晰的本地服务边界、以及符合直觉的 API 设计。**