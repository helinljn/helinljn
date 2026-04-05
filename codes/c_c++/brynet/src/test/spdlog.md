我给你做一份**面向“会用 + 能读懂核心源码”**的 **spdlog 1.15.3 学习计划**。

我会按下面几个目标来设计：

1. **会在项目里正确使用 spdlog**
2. **知道它的核心模块怎么协作**
3. **能顺着主调用链读源码**
4. **能自己扩展一个 sink / formatter**
5. **能理解同步日志和异步日志的实现思路**

---

# 一、你最终要达到的目标

如果学完这份计划，你应该能做到：

- 会用：
  - 控制台日志、文件日志、滚动日志、按天切分日志
  - 多 sink 输出
  - 自定义 pattern
  - 设置日志级别、flush 策略、错误处理
  - 使用同步/异步 logger
  - 使用宏 `SPDLOG_INFO / DEBUG / TRACE`
- 懂原理：
  - `logger` 是怎么把一条日志分发到多个 sinks 的
  - `sink`、`formatter`、`pattern_formatter` 的职责
  - `registry` 怎么管理全局 logger
  - 异步日志的线程池和队列怎么工作
  - `_mt` / `_st` 的区别
- 懂源码：
  - 能从 API 入口一路跟到真正写入文件/终端的位置
  - 能看懂关键类之间的关系
  - 能画出同步日志、异步日志两条主链路
- 会扩展：
  - 写一个自定义 sink
  - 写一个自定义 pattern flag
  - 根据项目需要封装自己的日志模块

---

# 二、建议前置知识

如果你已经有 C++ 基础，可以直接开始。最好具备这些能力：

## C++ 基础
- 类、继承、虚函数
- 模板基础
- `std::shared_ptr`、`std::unique_ptr`
- RAII
- `std::mutex`、`std::lock_guard`
- `std::thread`、`condition_variable`
- `std::chrono`
- `std::filesystem`（至少知道用途）

## 额外建议
- 看得懂一点 `fmt` 风格格式化：
  - `logger->info("value = {}", x);`
- 知道宏的基本用法：
  - `SPDLOG_INFO(...)`
  - `SPDLOG_ACTIVE_LEVEL`

如果你对 `fmt` 不熟，没关系，学 spdlog 的时候一起补就行。

---

# 三、先搭建学习环境

建议你先把环境准备好，不然源码阅读会很抽象。

## 1）拿到指定版本源码
建议直接 checkout 到 `v1.15.3`：

```bash
git clone https://github.com/gabime/spdlog.git
cd spdlog
git checkout v1.15.3
```

## 2）构建 examples / tests
建议至少把 examples 编出来。

如果你用 CMake：

```bash
cmake -S . -B build -DSPDLOG_BUILD_EXAMPLE=ON -DSPDLOG_BUILD_TESTS=ON
cmake --build build -j
```

## 3）准备一个自己的实验工程
你可以建一个小目录：

```text
spdlog-study/
  01_basic/
  02_sinks/
  03_pattern/
  04_registry/
  05_async/
  06_custom_sink/
  07_source_trace/
```

后面每学一个点就写一个最小 demo。

---

# 四、推荐的整体学习节奏

我建议你按 **6 周** 来学。
如果你每天能投入 **1~2 小时**，这个节奏比较舒服。
如果你时间更多，可以压缩到 3~4 周。

---

# 五、6 周学习计划

---

## 第 1 周：先把“会用”打牢

目标：**别一上来啃源码，先把 spdlog 当工具用顺手。**

---

### 学习内容
1. 安装和接入方式
   - header-only
   - compiled library
   - CMake 接入
2. 基础日志输出
   - `spdlog::info/debug/warn/error`
   - 创建命名 logger
3. 常见 sink
   - 控制台
   - 普通文件
   - rotating file
   - daily file
4. 日志级别与 flush
5. pattern 基础
6. 多 sink 组合

---

### 你要掌握的 API
- `spdlog::info(...)`
- `spdlog::stdout_color_mt(...)`
- `spdlog::basic_logger_mt(...)`
- `spdlog::rotating_logger_mt(...)`
- `spdlog::daily_logger_mt(...)`
- `logger->set_level(...)`
- `spdlog::set_level(...)`
- `logger->flush_on(...)`
- `spdlog::flush_every(...)`
- `logger->set_pattern(...)`

---

### 实践任务
至少写下面 5 个 demo：

#### demo1：最基础控制台日志
```cpp
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("Hello spdlog {}", 123);
    spdlog::warn("warning message");
}
```

#### demo2：文件日志
```cpp
auto logger = spdlog::basic_logger_mt("file_logger", "logs/basic.txt");
logger->info("file logger started");
```

#### demo3：滚动日志
```cpp
auto logger = spdlog::rotating_logger_mt("rotating", "logs/rotating.txt", 1024 * 1024, 3);
```

#### demo4：多 sink
一个 logger 同时输出到控制台和彩色文件/普通文件。

#### demo5：自定义格式
```cpp
spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
```

---

### 本周输出
你应该写出一份自己的笔记，至少回答：
- spdlog 最常用的 5 个 API 是什么？
- logger 和 sink 的关系是什么？
- pattern 在解决什么问题？
- `_mt` 和 `_st` 大致是什么意思？

---

## 第 2 周：建立源码全局地图

目标：**知道代码在哪里、每个模块负责什么。**

这一步非常关键。很多人读源码失败，不是不会读，而是**没有地图**。

---

### 先建立架构认知

你可以先把 spdlog 想成这样：

```text
用户代码
   ↓
spdlog API / 宏
   ↓
logger
   ↓
formatter
   ↓
1..N 个 sinks
   ↓
终端 / 文件 / 其他输出目标
```

异步模式则是：

```text
用户线程
   ↓
async_logger
   ↓
thread_pool + queue
   ↓
后台线程
   ↓
sinks
```

---

### 重点阅读的头文件顺序

建议按这个顺序看，不要乱跳：

#### 第一层：顶层入口
- `include/spdlog/spdlog.h`
- `include/spdlog/common.h`

作用：
- 暴露常见 API
- 定义基础类型、日志级别、宏、配置项

#### 第二层：核心 logger
- `include/spdlog/logger.h`

作用：
- 最核心的日志对象
- 负责接收日志调用、判断级别、构造 log message、分发给 sinks

#### 第三层：sink 抽象
- `include/spdlog/sinks/sink.h`
- `include/spdlog/sinks/base_sink.h`

作用：
- sink 接口
- 线程安全封装
- 具体 sink 的公共行为

#### 第四层：格式化
- `include/spdlog/pattern_formatter.h`
- 相关 details 目录中的 formatter 实现

作用：
- 解析 `%Y %m %d %l %v %t` 这些 pattern
- 把 `log_msg` 变成最终字符串

#### 第五层：日志消息结构
- `include/spdlog/details/log_msg.h`
- `include/spdlog/details/log_msg_buffer.h`

作用：
- 日志在内部传递时的数据结构

#### 第六层：全局管理
- `include/spdlog/details/registry.h`

作用：
- 注册命名 logger
- 默认 logger
- 全局配置

#### 第七层：异步
- `include/spdlog/async.h`
- `include/spdlog/async_logger.h`
- `include/spdlog/details/thread_pool.h`
- `include/spdlog/details/mpmc_blocking_q.h`

作用：
- 异步日志
- 队列
- 后台线程池

#### 第八层：文件与系统细节
- `include/spdlog/details/file_helper.h`
- `include/spdlog/details/os.h`

作用：
- 文件打开、重命名、路径处理
- 平台相关操作

---

### 本周阅读任务
你不需要逐行读完，只要完成这三件事：

#### 任务 1：画模块图
手画也行，电子版也行。
画出：
- API 层
- logger
- sink
- formatter
- registry
- async

#### 任务 2：写“模块职责表”
例如：

| 模块 | 作用 |
|---|---|
| logger | 日志分发核心 |
| sink | 具体输出目标 |
| formatter | 把日志对象格式化成字符串 |
| registry | 管理全局 logger |
| async_logger | 把日志交给线程池异步处理 |

#### 任务 3：回答下面 5 个问题
- 为什么一个 logger 可以挂多个 sink？
- 为什么 sink 和 formatter 要分开？
- registry 为什么存在？
- 为什么会有 `_mt` 和 `_st` 两套？
- 异步 logger 为什么还需要 sink？

---

## 第 3 周：深入同步日志主链路

目标：**能从一次 `info()` 调用跟到最终输出。**

这是你理解源码的第一条主线。

---

### 要重点读的源码
- `spdlog.h`
- `logger.h`
- `details/log_msg.h`
- `sinks/sink.h`
- `sinks/base_sink.h`
- 一个具体 sink，例如：
  - `stdout_color_sinks.h`
  - `basic_file_sink.h`
  - `rotating_file_sink.h`

---

### 重点要理解的问题

#### 1）一次日志调用的主流程
你要尝试跟这条链：

```cpp
logger->info("value={}", x);
```

大致理解成：

```text
info(...)
  → log(level, ...)
  → 判断 should_log(level)
  → 构造 log_msg
  → 调用 sink_it_(msg)
  → 遍历所有 sinks
  → sink->log(msg)
  → formatter->format(msg, buffer)
  → 写终端/文件
```

你不必执着于函数名一模一样，但要理解**职责链**。

---

#### 2）`log_msg` 是什么
你要知道它通常包含什么：
- logger 名称
- level
- 时间戳
- 线程 ID
- source location（可选）
- payload/message

它是内部传递的“日志载体”。

---

#### 3）为什么 sink 不直接接收原始格式参数
因为 spdlog 需要先把日志调用统一成内部消息，再做：
- 级别判断
- 格式化
- 分发
- 异步投递

---

#### 4）`_mt` 和 `_st` 的实现思路
通常：
- `_mt`：多线程安全，内部加锁
- `_st`：单线程，不加锁，更轻

在 `base_sink` 这条线上你会看得更清楚。

---

### 实践任务
#### 任务 1：自己画同步调用时序图
例如：

```text
main
 ↓
logger->info
 ↓
logger::log
 ↓
logger::sink_it_
 ↓
sink->log
 ↓
formatter->format
 ↓
write
```

#### 任务 2：写一个最小“多 sink logger”
- 控制台 + 文件
- 不同 sink 设置不同级别

#### 任务 3：验证 flush 行为
试试：
- `flush_on(spdlog::level::err)`
- 手动 `flush()`
- 看文件实际落盘效果

---

### 本周验收标准
如果我问你：

> “spdlog 中 logger、sink、formatter 分别负责什么？”

你应该能在 1 分钟内说清楚。

---

## 第 4 周：深入 formatter 和 pattern_formatter

目标：**看懂格式化层的源码与扩展点。**

很多人会用 `set_pattern()`，但不知道它内部怎么实现。

---

### 要重点读的源码
- `include/spdlog/pattern_formatter.h`
- 相关 formatter/details 实现
- 看一个具体 sink 如何使用 formatter

---

### 你要理解的内容

#### 1）pattern 是怎么工作的
例如：

```cpp
[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v
```

内部不是每次都重新“现解析字符串”，而是会把 pattern 编译成一组格式化动作/flag formatter，之后重复使用。

你要重点理解这个思想：

> **pattern string → 编译为 formatter 链 → 对每条日志执行格式化**

这能解释为什么它性能还不错。

---

#### 2）常见占位符的来源
- `%v`：日志正文
- `%l`：日志级别
- `%n`：logger 名称
- `%t`：线程 ID
- `%Y %m %d %H %M %S %e`：时间相关

#### 3）formatter 和 sink 的关系
- sink 通常持有 formatter
- sink 接到 `log_msg` 后，先格式化，再输出

#### 4）自定义 formatter 的两个层次
- 简单方式：`set_pattern()`
- 高级方式：自定义 formatter 或自定义 flag

---

### 实践任务

#### 任务 1：试 5 种 pattern
例如：
- 简洁模式
- 带线程号
- 带 logger 名称
- 带源码位置（如启用 source_loc）
- 带颜色

#### 任务 2：写一个自定义 pattern flag（如果你想更深入）
例如加一个固定业务字段或简化版 trace id。

就算不正式实现，也要知道扩展点在哪。

#### 任务 3：比较 pattern 改动前后的输出
观察：
- 可读性
- 调试便利性
- 是否适合生产

---

### 本周验收标准
你应该能回答：
- `set_pattern()` 背后大概做了什么？
- formatter 为什么不放在 logger 里统一做？
- sink 为什么通常拥有自己的 formatter？

---

## 第 5 周：深入 registry、全局 API、宏与配置

目标：**把“平时最常用但最容易忽略”的部分吃透。**

这一周会让你对“为什么 spdlog 用起来这么顺手”有更深理解。

---

### 要重点读的源码
- `include/spdlog/spdlog.h`
- `include/spdlog/details/registry.h`
- `include/spdlog/common.h`
- 和宏、source location 相关的部分

---

### 重点理解内容

#### 1）默认 logger 与全局 API
为什么你可以直接写：

```cpp
spdlog::info("hello");
```

因为内部有默认 logger / registry 的支持。

你要弄明白：
- 默认 logger 从哪里来
- 命名 logger 怎么注册
- `spdlog::get("name")` 怎么工作

---

#### 2）全局配置如何传播
例如：
- 全局 level
- 全局 pattern
- flush 策略

你要理解 registry 在其中扮演的角色。

---

#### 3）日志宏
重点看这些：
- `SPDLOG_TRACE`
- `SPDLOG_DEBUG`
- `SPDLOG_INFO`

以及：
- `SPDLOG_ACTIVE_LEVEL`

要理解：

> 有些日志宏可以在编译期裁剪，避免低级别日志带来运行期开销。

这是实际项目里非常重要的一点。

---

#### 4）source location
如果使用：
- 文件名
- 行号
- 函数名

那么内部会借助 `source_loc` 之类的结构传递。

---

#### 5）错误处理
要知道：
- sink 写失败会怎样
- 如何设置 error handler
- 生产环境中不能让日志库把主业务拖死

---

### 实践任务
#### 任务 1：试 registry API
- 创建命名 logger
- `spdlog::register_logger`
- `spdlog::get`
- `spdlog::drop`
- `spdlog::drop_all`

#### 任务 2：试日志宏
分别编译：
- 开启 debug/trace
- 关闭 debug/trace

观察二进制行为和输出差异。

#### 任务 3：开启源码位置信息
输出：
- 文件名
- 行号
- 函数名

---

### 本周验收标准
你应该能解释：
- `spdlog::info(...)` 为什么不用先显式创建 logger 也能工作？
- `SPDLOG_ACTIVE_LEVEL` 有什么价值？
- 为什么生产环境不能无脑开 trace？

---

## 第 6 周：深入异步日志与自定义扩展

目标：**真正达到“源码层面理解 + 能自己扩展”的程度。**

---

### 要重点读的源码
- `include/spdlog/async.h`
- `include/spdlog/async_logger.h`
- `include/spdlog/details/thread_pool.h`
- `include/spdlog/details/mpmc_blocking_q.h`
- 可补充看：
  - `details/backtracer.h`
  - `details/file_helper.h`

---

### 异步日志必须理解的点

#### 1）异步的主流程
你要建立这条链：

```text
用户线程调用 logger
  ↓
async_logger 不直接写 sink
  ↓
把消息投递到 thread_pool/queue
  ↓
后台线程取消息
  ↓
真正调用 sink 输出
```

---

#### 2）为什么异步 logger 仍然需要 sink
因为“异步”解决的是：
- 调用线程不要阻塞在 IO 上

但最终日志还是要落到某个地方：
- 控制台
- 文件
- 网络
- 其他系统

sink 仍然是最终输出层。

---

#### 3）队列满了怎么办
通常要理解溢出策略，例如：
- 阻塞等待
- 丢最老消息 / 覆盖旧消息

你要知道这涉及：
- 吞吐
- 时延
- 数据完整性

这是生产环境选型时的关键点。

---

#### 4）异步不一定永远更快
你要知道异步的收益和代价：
- 收益：主线程更轻、IO 脱离主路径
- 代价：额外线程、队列、顺序语义、退出时 flush 问题

---

### 自定义扩展任务

#### 任务 1：写一个自定义 sink
最推荐这个练手方式。

比如写一个：
- 把日志写到 `std::vector<std::string>` 的 sink
- 或者写一个简单“网络发送 mock sink”
- 或者写一个“按关键字过滤后输出”的 sink

你会真正理解：
- sink 接口
- formatter 的使用
- `_mt/_st` 的设计意义

---

#### 任务 2：同步和异步性能对比
写个简单 benchmark：
- 同步控制台 / 同步文件
- 异步文件
- 单线程、多线程

不用追求绝对准确，重点是观察现象。

---

#### 任务 3：读 tests/examples
源码学习里，**tests 是非常好的“行为文档”**。
建议你用这个方法读：
- 先看一个特性在 examples/tests 里怎么用
- 再回头跟实现

这样会轻松很多。

---

### 本周验收标准
你应该能说清：
- async_logger 和 logger 的关键差异
- thread_pool 在异步架构中的位置
- 队列满时为什么会有不同策略
- 什么时候不建议用异步日志

---

# 六、源码阅读推荐顺序

如果你只想抓主干，我给你一个最实用的顺序：

## 第一轮：只看主线，不抠细节
1. `spdlog.h`
2. `common.h`
3. `logger.h`
4. `sinks/sink.h`
5. `sinks/base_sink.h`
6. `pattern_formatter.h`
7. `details/log_msg.h`
8. `details/registry.h`
9. `async.h`
10. `async_logger.h`
11. `details/thread_pool.h`

这一轮的目标不是“看懂每一行”，而是：
- 知道入口
- 知道对象关系
- 知道主调用链

---

## 第二轮：带着问题精读
重点问题如下：

### logger 相关
- 日志级别在哪里判断？
- 一个 logger 怎么管理多个 sinks？
- flush 什么时候触发？

### sink 相关
- sink 接口长什么样？
- 为什么 base_sink 能同时支持 `_mt/_st`？
- 具体 sink 怎么复用 formatter？

### formatter 相关
- pattern 怎么被“编译”？
- 每个占位符如何映射到一个格式化动作？

### registry 相关
- 默认 logger 怎么初始化？
- 全局设置如何同步到 logger？

### async 相关
- enqueue 的消息是什么？
- worker 线程从哪开始跑？
- 队列与线程池如何配合？

---

## 第三轮：挑 2 个具体实现深挖
建议选：
- `rotating_file_sink`
- `async_logger`

因为这两个最能代表 spdlog 的价值。

---

# 七、你必须亲手做的 8 个小实验

这部分非常重要。
**源码不是光看会的，必须配合实验。**

---

## 实验 1：最小 logger
创建一个命名 logger，输出到控制台。

## 实验 2：多 sink
一个 logger 同时输出控制台和文件，设置不同 pattern。

## 实验 3：rotating file
观察日志滚动行为，看看文件命名和切换时机。

## 实验 4：daily file
验证按天切分。

## 实验 5：全局 level 与局部 level
观察 logger 本地设置和全局设置的关系。

## 实验 6：宏裁剪
改 `SPDLOG_ACTIVE_LEVEL`，观察 debug/trace 是否还存在。

## 实验 7：异步 logger
用线程池创建 async logger，测试高频写入。

## 实验 8：自定义 sink
哪怕只写一个把日志 push 到容器里的 sink，都非常值。

---

# 八、学习时最应该关注的“源码核心概念”

你读源码时，不要只看“函数在干嘛”，更要盯住这些设计点：

---

## 1）分层设计
spdlog 的核心分层很清晰：

- **logger**：组织日志流程
- **formatter**：格式化日志
- **sink**：负责输出
- **registry**：管理全局 logger
- **async layer**：解耦调用线程与 IO

这是它易用又可扩展的关键。

---

## 2）性能意识
spdlog 不是“能打印日志”这么简单，它很重视性能：

- 尽量减少无谓格式化
- 编译期裁剪低级别日志
- header-only 方便接入
- 支持异步
- `_mt/_st` 分版本减少锁开销

你读源码时要一直问：

> “这一层设计是为了性能、扩展性，还是易用性？”

---

## 3）模板 + 头文件实现
spdlog 大量是头文件实现。
所以你会看到很多 inline / template 风格代码。
这是正常的，不要被吓到。

---

## 4）对 fmt 的依赖
很多真正的字符串格式化工作，底层会借助 `fmt`。
所以你不必试图把“所有格式化细节都在 spdlog 内部看懂”。

你的目标应该是：
- spdlog 如何组织日志
- 何时调用 fmt
- 格式结果如何交给 sink

而不是去深挖 fmt 的全部源码。

---

# 九、常见难点和应对方法

---

## 难点 1：头文件太多，不知道从哪进
### 解决
按我上面给你的顺序：
- `spdlog.h`
- `logger.h`
- `sink.h`
- `base_sink.h`
- `pattern_formatter.h`
- `registry.h`
- `async_logger.h`

不要一上来钻 `details` 里的所有文件。

---

## 难点 2：函数跳来跳去
### 解决
只追两条线：
- 同步主线
- 异步主线

其他都先放掉。

---

## 难点 3：看源码看着看着忘了为什么这样设计
### 解决
每读完一个模块，就写这 4 件事：
1. 它负责什么
2. 它依赖谁
3. 谁会调用它
4. 它暴露了什么扩展点

---

## 难点 4：异步部分不好懂
### 解决
先用图来理解，不要先抠代码。

先在脑子里建立这个图：

```text
producer threads
   ↓
queue
   ↓
thread pool workers
   ↓
sinks
```

然后再回头看 `async_logger` 和 `thread_pool`。

---

# 十、最终验收项目

建议你在学完之后做一个**小型日志封装项目**，作为毕业作业。

---

## 项目目标
封装一个你自己的日志模块 `MyLogger`，要求：

### 基础功能
- 控制台 + 滚动文件双输出
- 支持全局 level 设置
- 支持 pattern 设置
- 支持同步 / 异步切换
- 支持源码位置输出
- 支持程序启动时初始化

### 进阶功能
- 自定义 sink 或者自定义 flag
- 提供业务宏：
  - `LOG_INFO`
  - `LOG_WARN`
  - `LOG_ERROR`
- 支持 debug/trace 编译期开关

---

## 项目完成后，你应该能回答：
- 为什么你的封装要保留 spdlog 的 logger 概念？
- 为什么要把 sink 配置做成组合式？
- 为什么生产环境默认建议文件日志而不是控制台？
- 为什么高频场景可能考虑异步？
- 异步下退出程序前为什么要注意 flush/shutdown？

---

# 十一、建议的学习资料使用顺序

建议你按这个顺序配合使用：

1. **官方 README / examples**
   - 先会用
2. **tests**
   - 看行为
3. **核心头文件**
   - 看主线
4. **具体 sink 实现**
   - 看落地细节
5. **async 相关源码**
   - 看高阶特性

---

# 十二、给你的“最短路径版”学习路线

如果你想先快速进入状态，可以直接按下面做：

## 第 1 步
先会写这几类代码：
- 控制台日志
- 文件日志
- rotating file
- 多 sink
- async logger

## 第 2 步
只读这 6 个文件：
- `spdlog.h`
- `logger.h`
- `sink.h`
- `base_sink.h`
- `pattern_formatter.h`
- `async_logger.h`

## 第 3 步
自己画两张图：
- 同步日志调用图
- 异步日志调用图

## 第 4 步
写一个自定义 sink

做到这一步，你基本就已经达到：

> “会用，并且理解核心源码结构”

---

# 十三、我建议你每天怎么学

如果你每天学 1~2 小时，可以这样安排：

## 每天固定节奏
- 20 分钟：看 examples / 文档
- 30 分钟：读源码
- 30 分钟：写 demo 或实验
- 10 分钟：记笔记

## 笔记模板
每个模块都用这 5 个问题记：

1. 这个模块的职责是什么？
2. 它的核心类有哪些？
3. 它和哪些模块协作？
4. 它的扩展点是什么？
5. 它的性能/线程安全设计点是什么？

---

# 十四、最后给你一个非常实用的阅读清单

你可以把下面当成 checklist：

---

## 使用层
- [ ] 会创建控制台 logger
- [ ] 会创建文件 logger
- [ ] 会创建 rotating/daily logger
- [ ] 会设置 pattern
- [ ] 会设置 level
- [ ] 会设置 flush
- [ ] 会创建 async logger
- [ ] 会组合多个 sinks

## 源码层
- [ ] 知道 `logger` 是核心分发器
- [ ] 知道 `sink` 是输出抽象
- [ ] 知道 `formatter` 是格式化层
- [ ] 知道 `registry` 管理全局 logger
- [ ] 知道 `log_msg` 是内部载体
- [ ] 知道异步模式依赖 thread pool + queue

## 扩展层
- [ ] 能写一个自定义 sink
- [ ] 知道如何做自定义 formatter/flag
- [ ] 知道如何封装自己的日志模块