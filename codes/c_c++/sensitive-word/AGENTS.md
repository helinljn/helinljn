# AGENTS.md

本文件为后续编码代理提供本仓库的项目上下文和协作规则，帮助代理安全、准确地修改代码。

## 项目概览

`sensitive-word` 是一个基于 Modern C++（C++17）的高性能敏感词检测库，核心能力包括：

- Word 检测
- Number 检测
- 文本归一化与绕过对抗
- 黑白名单管理
- 匹配结果提取、过滤与替换

项目重点是生产可用、低拷贝、热路径性能和复杂 Unicode 归一化场景。修改核心匹配、归一化、替换和词库装载逻辑时，默认认为兼容性和性能风险较高。

## 构建系统

本项目使用 CMake 3.15+ 与 C++17。

主要目标：

| 目标 | 类型 | 源码 | 说明 |
|------|------|------|------|
| `core` | 动态库 | `src/core/` | 基础设施与 `src/core/sw/` 敏感词核心能力。链接 funchook、jsoncpp、opencc；Windows 还链接 Dbghelp，Linux 还链接 dl。 |
| `test` | 可执行文件 | `src/test/` | doctest 单元测试，链接 `core` 与 `mimalloc`。 |
| `benchmark` | 可执行文件 | `src/benchmark/` | nanobench 基准测试，链接 `core` 与 `mimalloc`。 |
| `hcode` | 可执行文件 | `src/hcode/hcode/` | Hook/热补丁实验验证目标，链接 `mimalloc`、`core`、`testa`。 |
| `testa` | 动态库 | `src/hcode/testa/` | Hook 实验辅助库。 |
| `testpatch` | 动态库 | `src/hcode/testpatch/` | Hook 实验补丁库。 |

重要约定：

- `src/core/sw/` 不是独立库目标，源码直接编译进 `core`。
- 修改 `src/core/sw` 会影响所有链接 `core` 的目标。
- 根 `CMakeLists.txt` 中的 `PROJECT_TARGET_APPLY_COMMON_OPTIONS` 统一应用平台编译选项。
- Windows 公共 C++ 编译选项包括 `/utf-8`、`/permissive-`、`/Zc:__cplusplus`、`/bigobj`、`/W4`、`/MP`、`/wd4251`。
- Linux 公共编译选项包括 `-Wall`、`-Wextra`、`-Wpedantic`。
- `compile_commands.json` 由 CMake 生成；需要编译数据库时优先使用构建目录中的文件。

## 常用命令

首次构建或第三方依赖缺失时：

```powershell
.\init-3rd.bat
```

Windows 构建：

```powershell
.\build.windows.bat release
.\build.windows.bat debug
.\build.windows.bat all
```

Linux/WSL 构建：

```bash
./init-3rd.sh
./build.linux.sh release
./build.linux.sh debug
./build.linux.sh all
```

运行普通单元测试：

```powershell
cd .build\Release
.\test.exe
```

运行基准测试：

```powershell
cd .build\Release
.\benchmark.exe
```

运行 hcode 测试：

```powershell
cd .build\Release
.\hcode.exe
```

构建目录与运行目录：

- Windows CMake 配置目录：`.build/windows/x64-Release`、`.build/windows/x64-Debug`
- Linux CMake 配置目录：`.build/linux/x64-Release`、`.build/linux/x64-Debug`
- 可执行文件和运行资源默认输出到 `.build/Release` 或 `.build/Debug`
- 构建过程会生成或复制 `data/config`、`data/dictionary` 和 `res/` 到运行目录；测试和基准通常应从运行目录启动。
- `init-3rd.*` 对已存在的第三方目录会执行 `git checkout .`、`git fetch` 和切换/拉取版本；如果 `3rd/` 内存在手工改动，运行前需要明确用户意图。

## 重要目录与文件

- `src/core/`：`core` 动态库的基础设施和敏感词核心模块。典型内容包括 `common.*`、base64、md5、datetime、symbol_loader、stack_trace、`sw/`。
- `src/core/sw/`：敏感词核心领域逻辑，包括公开 API、Trie、归一化、字符忽略、结果条件、替换策略、词库加载。
- `src/core/sw/sensitive_word.*`：公开 API、builder、engine、`word_result`、运行时增删词。
- `src/core/sw/text_normalizer.*`：UTF-8 解码、全半角折叠、大小写折叠、繁转简、数字样式与英文样式折叠。
- `src/core/sw/trie_dictionary.*`：Trie 字典树实现。
- `src/core/sw/char_ignore.*`：扫描时的字符跳过策略。
- `src/core/sw/result_condition.*`：命中后的条件过滤。
- `src/core/sw/replace_strategy.*`：替换文本生成策略。
- `src/core/sw/word_dictionary_loader.*`：deny / allow 词库加载。
- `src/test/`：doctest 单元测试，主入口为 `test.cpp`，核心行为测试集中在 `sensitive-word-test.cpp`。
- `src/benchmark/`：nanobench 性能验证。
- `src/hcode/`：Hook/热补丁实验，不要把这里的实验逻辑扩散到敏感词核心库。
- `res/`：敏感词资源，当前包含 `dict-2026-04-20.txt`，约 76k 词条。
- `scripts/optimize_dict.py`：词库清洗、去重、长度过滤和报告输出脚本，需要 `charset-normalizer` 与 `opencc-python-reimplemented`。
- `3rd/`：第三方依赖目录，默认视为外部代码。
- `3rd/3rd-builds/`：本仓库对 distorm、funchook、jsoncpp、mimalloc、opencc、OpenCC 数据和词典生成目标的 CMake 封装。

OpenCC 配置与二进制词典不在 `res/` 中，构建时会生成或复制到运行目录下的 `data/config` 与 `data/dictionary`。修改 OpenCC 资源路径时，需要同时检查 `text_normalizer` 搜索路径和构建后运行目录。

词库文本按行加载，首尾空白会被裁剪；空行、`#` 开头和 `//` 开头的行会被忽略。文件加载失败会抛出异常，相关行为测试位于 `src/test/sensitive-word-test.cpp`。

## 敏感词核心语义

- `sensitive_word_engine` 是公开值类型包装，内部使用 Pimpl。
- `sensitive_word_builder` 收集配置、词库和策略对象，并通过 `build()` 构建引擎。
- `char_ignore`、`result_condition`、`replace_strategy` 采用策略模式；工厂函数位于 `char_ignores`、`result_conditions`、`replace_strategies`。
- 默认 `char_ignore` 是 `none()`，不会跳过字符；需要穿透特殊符号、标点或零宽字符时使用 `char_ignores::special_chars()`。
- 默认 `result_condition` 是 `english_word_match()`，含 ASCII 字母的命中需要满足英文单词边界；需要子串命中时使用 `always_true()`。
- `english_word_num_match()` 同时要求 ASCII 字母和数字满足单词边界。
- 默认替换策略是 `replace_strategies::stars()`，也支持 `chars(char)` 和自定义 `replace_strategy`。
- 引擎支持运行时增删词：`add_word()`、`remove_word()`、`add_allow_word()`、`remove_allow_word()`。
- 同一引擎实例并发读写不默认线程安全；调用方需要外部同步，或构建新实例后替换引用。
- `query_word_status()` 返回词条是否存在于 deny / allow 字典。

匹配与扫描语义：

- 核心流程为：`text_normalizer` → 遍历归一化码点 → `char_ignore::ignore()` 跳过干扰字符 → Trie 搜索 → `result_condition::match()` 过滤 → 输出 `word_result` → `replace_strategy` 基于原始字节区间替换。
- `text_normalizer` 单次遍历完成 UTF-8 解码、全半角折叠、大小写折叠、繁转简、数字样式折叠和英文样式折叠。
- 繁转简按单个代码点执行；基础 CJK 区间使用连续数组缓存，扩展区字符使用映射缓存。不要把整段文本分词级 OpenCC 转换引入热路径。
- OpenCC 运行时仅从当前工作目录下的 `./data/config` 与 `./data/dictionary` 查找资源；资源缺失时繁简转换会保守降级，测试此能力时必须确认工作目录和数据文件存在。
- `ignore_repeat` 属于扫描层行为，不在 `text_normalizer_options` 中处理；重复字符尾部折叠不会改变归一化输出。
- `contains()` 内部固定使用最短匹配，不要破坏这个性能语义。
- `match_options::longest_match` 控制最短匹配与最长匹配；确认命中后扫描游标跳过已命中跨度，不输出交叉重叠的重复结果。
- allow / deny 同起点同时命中时，只有 deny 明确长于 allow 才判定 deny，否则 allow 覆盖并跳过 allow 跨度。
- Number 检测通过 `enable_num_check(true)` 开启，`num_check_len` 控制最小触发长度；数字检测复用归一化结果。

## 开发准则

- 默认目标标准为 C++17，不引入更高标准特性。
- 修改范围聚焦当前任务，不做无关重构、格式化或架构调整。
- 保持公共接口和既有行为稳定；必须改变时说明影响面和兼容性。
- 遵循现有命名、目录边界和性能导向。
- `src/core` 根层保持通用基础设施职责，不混入业务协议或临时实验逻辑。
- `src/core/sw` 承载敏感词领域逻辑，不引入 HTTP/JSON 等上层应用细节。
- 优先复用仓库已有工具函数，例如 `src/core/common.*` 中的字符串能力。
- 热路径优先低拷贝、低分配和缓存友好；大对象优先按引用或 `std::string_view` 传递。
- RAII 管理资源，动态资源优先使用 `std::unique_ptr` 或 `std::shared_ptr`。
- 删除未使用头文件、变量、死代码和残留调试逻辑。
- 注释只解释必要意图，不写重复代码含义的空泛注释。
- 新增或修改文本文件默认使用 UTF-8；修改已有文件时尽量保持原有行结束符风格。
- `3rd/` 下源码默认不直接修改；依赖初始化或升级应通过 `init-3rd.*` 或明确升级任务完成。

## 测试说明

普通核心变更至少运行：

```powershell
cd .build\Release
.\test.exe
```

涉及归一化、匹配、替换、词库装载或运行时增删词时，应补齐或更新 `src/test/sensitive-word-test.cpp` 中的行为测试。

涉及热路径性能、Trie、归一化或替换策略的明显改动时，视风险运行：

```powershell
cd .build\Release
.\benchmark.exe
```

涉及 Hook/热补丁实验目标时运行：

```powershell
cd .build\Release
.\hcode.exe
```

无法运行测试时，必须说明原因；不要假设测试通过。

## 第三方依赖

`init-3rd.bat` / `init-3rd.sh` 会拉取以下依赖：

| 依赖 | 版本 | 用途 |
|------|------|------|
| `brynet` | HEAD | 异步网络库，供 `core` 暴露头文件和网络相关测试使用 |
| `distorm` | HEAD | 反汇编库，供 `funchook` 使用 |
| `funchook` | HEAD | Hook/热补丁实验相关能力，`core` 当前链接该库 |
| `utfcpp` | v4.0.9 | UTF-8/16/32 迭代 |
| `opencc` | ver.1.2.0 | 繁简转换数据生成与相关资源 |
| `spdlog` | v1.15.3 | 日志 |
| `jsoncpp` | 1.9.7 | JSON 解析 |
| `doctest` | v2.4.12 | 测试框架 |
| `mimalloc` | v2.3.2 | 内存分配器，供 test/benchmark/hcode 链接 |
| `nanobench` | v4.3.11 | 微基准测试 |

构建 OpenCC 数据目标需要 Python3。OpenCC 数据会在构建输出目录下生成或复制到 `data/config` 与 `data/dictionary`。

## 代码审查关注点

审查时先列问题，再给结论；问题按严重程度排序。优先关注：

- bug、兼容性破坏、并发风险和状态不一致。
- 匹配、归一化、替换、词库装载等热路径性能回退。
- 核心行为、失败路径或边界条件缺失测试。
- 跨层依赖、重复逻辑、死代码和误导性注释。

## Git 协作

- 工作区可能存在无关本地改动。除非用户明确要求，不要回滚、覆盖或删除这些改动。
- 如果未预期改动与当前任务相关，基于现状继续；如果无关，忽略它们。
- 不要使用 `git reset --hard` 或 `git checkout --` 回滚用户改动，除非用户明确要求。
- 当构建命令、目录职责、核心语义或必要验证步骤发生变化时，同步更新 `AGENTS.md`。