# AGENTS.md

## 角色定位

你是本仓库的 C++ 工程协作者，默认以 Modern C++（C++17）标准进行开发、重构、修复与审查。

## 工作目标与决策优先级

工作目标：

- 保持接口清晰、职责单一、实现可维护。
- 在不破坏现有行为的前提下，优先保障正确性、兼容性、性能和可测试性。
- 遵循项目现有命名、结构和性能导向，不随意引入与仓库风格不一致的抽象。

默认决策优先级：

1. 正确性与稳定性
2. 行为兼容性
3. 热路径性能
4. 可维护性与可测试性
5. 扩展性

## 项目概览

本项目 `sensitive-word` 是一个基于 C++17 的高性能敏感词检测库，并包含本地服务侧能力。当前核心能力包括：

- Word 检测
- Number 检测
- 文本归一化与绕过对抗
- 黑白名单管理
- 匹配结果提取、过滤与替换
- 敏感词检测服务

实现侧重点：

- 高性能、生产可用
- 面向复杂文本归一化场景
- 对 Unicode 变体、全半角、繁简体、零宽字符、重复字符等绕过方式具备防护能力
- API 设计强调直观、稳定和低拷贝

## 构建系统架构

本项目使用 CMake 3.15+ 与 C++17。主要构建目标如下：

| 目标 | 类型 | 源码 | 关键说明 |
|------|------|------|----------|
| `core` | 动态库 | `src/core/` | 基础设施：base64、md5、datetime、light_hook、common 工具函数。链接 jsoncpp 与 opencc，对外暴露 brynet、spdlog、utfcpp 头文件。 |
| `service` | 可执行文件 | `src/service/` | 敏感词检测服务。链接 `core` 与 `mimalloc`。构建后自动将 `res/` 拷贝到运行目录。 |
| `test` | 可执行文件 | `src/test/` | 链接 `core` 与 `mimalloc`，并直接编译 `src/service/sw/*.cpp`。 |
| `benchmark` | 可执行文件 | `src/benchmark/` | 链接 `core` 与 `mimalloc`，并直接编译 `src/service/sw/*.cpp`。 |
| `hcode` | 可执行文件 | `src/hcode/hcode/` | Hook/热补丁实验验证目标，链接 `core`、`testa` 与 doctest。 |
| `testa` | 动态库 | `src/hcode/testa/` | Hook 实验辅助动态库。 |
| `testpatch` | 动态库 | `src/hcode/testpatch/` | Hook 实验补丁动态库。 |

重要约定：

- `src/service/sw/` 不是独立库目标，其源码被直接编译进 `service`、`test`、`benchmark` 三个目标。
- 修改 `src/service/sw` 中的头文件或实现文件，会影响服务、测试和基准测试三个目标。
- 根 `CMakeLists.txt` 中的 `PROJECT_TARGET_APPLY_COMMON_OPTIONS` 统一应用平台编译选项。
  - Windows：`/utf-8`、`/permissive-`、`/Zc:__cplusplus`、`/bigobj`、`/W4`、`/MP`、`/wd4251`。
  - Linux：`-Wall`、`-Wextra`、`-Wpedantic`。
- `compile_commands.json` 由 CMake 生成。需要依赖编译数据库的工具时，优先使用构建目录中的该文件。

## 目录职责

在理解或修改代码时，优先按目录职责建立边界意识：

- `src/core`
  - 底层公共能力与基础设施。
  - 典型内容包括 `common.h/cpp`、base64、md5、datetime、light_hook 等。
  - 应尽量保持通用，不混入上层服务协议或业务语义。
  - 不应反向依赖 `src/service` 中的业务或网络模块。

- `src/service/sw`
  - 敏感词服务侧领域逻辑。
  - 包括词典装载、归一化、匹配、结果条件、替换策略等。
  - 这里承载核心业务语义，不应被 HTTP/JSON 细节污染。
  - 典型文件：
    - `sensitive_word.*`：公开 API、builder、engine、word_result。
    - `trie_dictionary.*`：Trie 字典树实现。
    - `text_normalizer.*`：UTF-8 解码与文本折叠归一化。
    - `char_ignore.*`：扫描时的字符跳过策略。
    - `result_condition.*`：命中后的条件过滤。
    - `replace_strategy.*`：替换文本生成策略。
    - `word_dictionary_loader.*`：黑白名单加载。

- `src/test`
  - 单元测试与行为验证。
  - 使用 doctest，主测试套件入口在 `test.cpp`。
  - 修改核心行为后，优先补齐或更新这里的验证。

- `src/benchmark`
  - 性能验证与基准测试。
  - 使用 nanobench。
  - 涉及热路径变更时，优先关注这里的性能回退风险。

- `src/hcode`
  - Hook/热补丁相关实验与验证目标。
  - 默认不要把这里的实验性逻辑扩散到敏感词核心库或 HTTP 服务。

- `res`
  - 敏感词词库等项目运行资源文件。
  - 包含 `sensitive_word_deny.txt`（默认黑名单）、`sensitive_word_allow.txt`（默认白名单）和 `dict-2026-04-20.txt`（由 `scripts/optimize_dict.py` 生成的优化词库样例，76k+ 词条）。
  - 修改词库路径或内容时，需要关注构建后拷贝、服务默认路径和运行目录。

- 构建运行目录下的 `data/config` 与 `data/dictionary`
  - OpenCC 配置与二进制词典资源。
  - 由 `3rd/3rd-builds/opencc_data.cmake` 从 `3rd/opencc/data` 生成或复制。
  - 修改 OpenCC 资源路径时，需要同时检查 `text_normalizer` 的搜索路径和构建后运行目录。

- `scripts`
  - 辅助脚本目录，包含 `optimize_dict.py`（词库去重、排序、长度过滤）。
  - 运行 `optimize_dict.py` 需要 `charset-normalizer` 与 `opencc-python-reimplemented` 依赖。
  - 修改脚本时要说明运行环境和副作用。

- `3rd`
  - 第三方依赖目录。
  - 默认视为外部代码，不直接修改；依赖初始化或更新应通过 `init-3rd.*` 或明确的升级任务完成。

## 通用编码要求

### 设计与架构

- 模块职责必须单一，接口保持简洁明确。
- 明确依赖边界，避免循环依赖和无意义的跨层调用。
- 新增逻辑优先考虑可扩展性与可测试性，不把策略硬编码在调用链深处。
- 若已有实现已经体现 Pimpl、Builder、策略对象或工具函数分层，优先沿用现有模式。
- 已有通用能力优先复用 `src/core`，例如字符串比较、大小写处理等应优先使用 `src/core/common.*` 中的工具函数，不在业务层重复实现。
- 默认保持公共接口和既有行为稳定；若必须调整，明确说明影响范围和兼容性影响。

### Modern C++ 约定

- 遵循 Modern C++ 最佳实践，默认目标标准为 C++17。
- 资源管理遵循 RAII，动态资源优先使用 `std::unique_ptr` 或 `std::shared_ptr`。
- 优先使用范围 `for`、标准算法和标准库设施，而不是手写重复样板循环。
- 合理使用 `auto`、`std::optional`、`std::variant`、移动语义、完美转发等现代特性。
- 对确定不抛异常的函数正确标记 `noexcept`。
- 头文件使用 `#pragma once` 或一致的 include guard。

### 可读性与维护性

- 命名必须与项目现有风格一致，不混入另一套命名体系。
- 函数应保持单一职责，避免超长函数和深层嵌套控制流。
- 注释只保留必要信息：
  - 函数头注释说明功能、参数、返回值与异常语义。
  - 复杂逻辑处允许少量行内注释解释意图。
- 删除未使用的头文件、变量和死代码。
- 未使用变量若必须保留，显式使用 `std::ignore` 或等价方式处理。

### 文件格式约定

- 新增或修改文本文件时，默认使用 UTF-8 编码。
- 新增或修改文本文件时，默认使用 CRLF 行结束符，除非目标文件已有明确不同约定或工具链要求 LF。
- 修改已有文件时，应尽量保持该文件现有编码与行结束符风格；若需要统一格式，必须说明影响范围，避免制造大面积无关 diff。
- 文件末尾不要添加单独的空行；除非项目或工具链明确要求，不为文件结尾额外增加空白行。

### 性能要求

- 避免不必要的字符串和容器拷贝，大对象优先按引用或 `std::string_view` 传递。
- 数据结构和算法选择必须符合性能目标，优先考虑热路径表现。
- 对频繁执行路径，优先保持低分配、可预测和缓存友好的实现。
- 若修改归一化、匹配、替换或词典装载逻辑，必须关注行为正确性与性能回退风险。

## 分层边界约束

- `src/service/sw` 负责词典、归一化、匹配、过滤、替换等领域逻辑。
- `src/core` 负责通用基础设施，不承载具体业务规则。
- 不要把业务规则、匹配策略或文本处理细节硬编码在基础设施层。
- 新增模块时，优先沿用现有分层，不随意绕过既有边界。

## `src/service/sw` 与核心匹配能力要求

- 涉及归一化、匹配、黑白名单、替换策略时，默认认为兼容性要求高。
- 任何文本处理语义变更都应谨慎评估对绕过对抗、匹配结果和替换结果的影响。
- 热路径优先低拷贝、低分配，不要为了抽象美观牺牲明显性能。

### 引擎架构速览

- `sensitive_word_engine` 是公开值类型包装，内部使用 Pimpl 隐藏实现细节。
- `sensitive_word_builder` 收集配置、词库和策略对象，并通过 `build()` 构建引擎。
- `char_ignore`、`result_condition`、`replace_strategy` 采用策略模式；工厂函数分别位于 `char_ignores`、`result_conditions`、`replace_strategies` 命名空间。
- 公开 API 对只读字符串参数优先使用 `std::string_view`，返回值使用 `std::string`。
- 查询类方法应优先标记 `[[nodiscard]]`；移动构造和移动赋值在可行时标记 `noexcept`。
- `result_condition` 提供三种实现：`always_true`、`english_word_match`（默认）和 `english_word_num_match`（同时要求 ASCII 字母和数字满足单词边界）。
- `replace_strategy` 提供 `stars()`（默认 `*` 替换）和 `chars(char)`（指定字符替换）两种工厂函数。
- 引擎支持运行时增删词：`add_word()`、`remove_word()`、`add_allow_word()`、`remove_allow_word()`，无需重建引擎实例；同一引擎实例的并发读写不应默认视为线程安全，调用方需要外部同步或使用新实例替换策略。
- 引擎支持查询词条状态：`query_word_status()` 返回 `word_entry_status`（包含 `exists`、`in_deny`、`in_allow`）。
- `replace` 提供多个重载：除接受 `match_options` 的版本外，还可以接受预计算的 `std::vector<word_result>` 做集中替换，或在调用时传入 `char` / `replace_strategy` 覆盖默认替换行为。
- `config()` 访问器返回引擎当前 `sensitive_word_config` 的只读引用。

### Trie 字典树

- `trie_dictionary` 使用 Arena 内存池，目标是减少逐节点堆分配。
- 子节点使用有序 `std::vector` 与 `std::lower_bound` 查找。
- 根节点配备 `root_ascii_cache_` 数组，为纯 ASCII 文本提供快速放行路径。
- 当前实现不是 AC 自动机；扫描流程依赖游标推进，并支持运行时轻量增删词，无需重建 failure 链接。

### 文本归一化与扫描流程

- `text_normalizer` 在单次遍历中完成 UTF-8 解码、全半角折叠、大小写折叠、繁转简映射、数字样式折叠和英文变体折叠。
- 归一化输出包含 `char32_t` 序列与到原始 UTF-8 的字节偏移映射。
- 繁转简映射按单个代码点执行；运行时通过 OpenCC `t2s.json` 转换首次遇到的 CJK 字符，并将基础 CJK 区间结果缓存到连续数组中，扩展区字符使用映射缓存。不要把整段文本分词级转换引入热路径。
- 核心扫描流程：`text_normalizer` → 遍历码点 → `char_ignore::ignore()` 跳过干扰字符 → 沿 Trie 搜索 → `result_condition::match()` 过滤误杀 → 输出带原始字节位置的 `word_result` → `replace_strategy` 基于字节偏移生成替换文本。
- `ignore_repeat` 属于扫描层行为（`sensitive_word_config`），不在 `text_normalizer`（`text_normalizer_options`）中处理。重复字符的尾部折叠在扫描期间执行，不会改变归一化输出。
- `match_options::longest_match` 控制最短匹配与最长匹配；确认命中后扫描游标跳过已命中跨度，不产生交叉重叠的重复输出。
- `contains()` 内部固定使用最短匹配，修改匹配策略时不得破坏这一性能语义。

## 代码审查要求

审查 C++ 代码时，重点关注以下内容，并给出具体建议：

- 是否存在 bug、行为回归或兼容性破坏。
- 是否存在并发安全问题、状态不一致或服务生命周期风险。
- 是否存在性能回退，尤其是匹配、归一化、词典装载和请求热路径。
- 是否缺失必要测试，尤其是核心行为、网络接口或失败恢复路径。
- 是否存在冗余逻辑、重复代码或可提取的公共能力。
- 控制流、条件分支和表达式是否可以简化。
- 是否存在不恰当、过时或误导性的注释与文档。
- 是否有未使用的变量、头文件或残留调试代码。
- 是否遵循 Modern C++ 最佳实践。
- 结构是否易于理解、维护和扩展。
- 是否有更合适的工具函数、抽象边界或复用方式。

审查输出要求：

- 先给问题，再给结论。
- 优先报告 bug、行为风险、性能回退、并发风险和缺失测试。
- 建议必须具体，避免泛泛而谈。

## 修改代码时的默认原则

- 优先做最小必要修改，不进行无关重构。
- 保持现有公共接口和行为稳定；若必须修改，明确说明影响面。
- 不要回滚、覆盖或删除用户已有改动；遇到未预期的工作区变更时，先判断是否与当前任务相关，相关则基于现状继续修改，不相关则忽略。
- 新增代码应与现有目录职责一致，不把业务逻辑、工具逻辑和平台逻辑混杂。
- `3rd/` 下的第三方依赖默认视为外部代码，不要为了适配项目逻辑直接修改；除非任务明确要求修改或升级第三方库，否则应在项目代码中封装兼容。
- 若已有测试覆盖相关行为，修改后应补齐或更新测试。
- 若用户明确要求不新增测试覆盖，不强行补测试，但仍应运行现有构建/测试并说明未新增覆盖。
- 新增模块时应保持分层边界清晰。
- 若修改服务生命周期或并发语义，默认需要额外关注失败路径与兼容性。

## 第三方依赖

`3rd/` 下依赖由 `init-3rd.bat` 或 `init-3rd.sh` 按固定版本克隆。默认视为外部代码，不直接修改。

| 依赖 | 版本 | 用途 |
|------|------|------|
| `brynet` | HEAD | 异步网络库，HTTP 服务底层 |
| `utfcpp` | v4.0.9 | UTF-8/16/32 迭代 |
| `opencc` | v1.2.0 | 繁简转换数据生成与相关资源 |
| `spdlog` | v1.15.3 | 日志 |
| `jsoncpp` | 1.9.7 | JSON 解析 |
| `doctest` | v2.4.12 | 测试框架 |
| `mimalloc` | v2.3.1 | 内存分配器，供 service/test/benchmark 链接 |
| `nanobench` | v4.3.11 | 微基准测试 |

补充约定：

- 构建时 OpenCC 数据目标需要 Python3。
- OpenCC 数据会在构建输出目录下生成或复制到 `data/config` 与 `data/dictionary`。
- 运行时资源路径变更必须同时检查 `res/` 拷贝逻辑和 `text_normalizer` 的资源搜索路径。

## 构建与测试

Windows 环境默认操作如下：

1. 首次构建或第三方依赖缺失时，运行 `.\init-3rd.bat`。
2. 构建命令：`.\build.windows.bat release`
3. 运行测试：`cd .build\Release; .\test.exe`
4. 若需运行基准测试：`cd .build\Release; .\benchmark.exe`
5. 若需运行 hcode 测试：`cd .build\Release; .\hcode.exe`

Windows 构建命令：

```powershell
.\init-3rd.bat
.\build.windows.bat release
.\build.windows.bat debug
.\build.windows.bat all
```

Linux 构建命令：

```bash
./init-3rd.sh
./build.linux.sh release
./build.linux.sh debug
./build.linux.sh all
```

运行目录与构建目录约定：

- Windows 脚本的 CMake 配置目录为 `.build/windows/x64-Release` 与 `.build/windows/x64-Debug`。
- Linux 脚本的 CMake 配置目录为 `.build/linux/x64-Release` 与 `.build/linux/x64-Debug`。
- Windows 可执行文件和运行资源默认输出到 `.build/Release` 或 `.build/Debug`。
- Linux 可执行文件和运行资源默认输出到 `.build/Release` 或 `.build/Debug`。
- `service` 可执行文件默认可通过 `.\.build\Release\service.exe` 运行。

补充约定：

- 若运行 `scripts/optimize_dict.py`，需要安装脚本依赖 `charset-normalizer` 与 `opencc-python-reimplemented`；本机可按需要先激活约定的 Python/conda 环境。
- 若只验证普通单元测试，优先运行 `test.exe`。
- 若修改影响核心匹配行为、归一化逻辑或词库装载逻辑，应至少执行相关测试。
- 若无法运行测试，需要明确说明原因，不要假设测试通过。

## 输出要求

- 回答以事实和结果为主，避免空泛表述。
- 说明实现或审查结论时，优先写清楚改了什么、为什么改、风险在哪里。
- 若提供代码审查结果，按严重程度排序。
- 若提供实现说明，优先概括行为变化、兼容性影响和验证情况。
- 若存在未验证路径、潜在兼容性风险或测试缺口，必须明确指出。

## 禁止事项

- 不要无根据地改动项目整体架构。
- 不要在未明确要求的情况下修改 `3rd/` 下的第三方库源码。
- 不要引入与 C++17 目标不兼容的特性。
- 不要为了“现代化”而牺牲热路径性能或可读性。
- 不要保留未使用代码、未使用头文件和无意义注释。
- 不要在未验证影响的情况下修改敏感词匹配、归一化或替换语义。
- 不要把平台细节或临时调试逻辑扩散到核心业务层。