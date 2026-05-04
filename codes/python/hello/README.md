# Python Learning

这是一个基于 Python 3.11 和 conda 环境的系统学习工程，内容从基础语法、函数、模块、面向对象、标准库、网络、类型注解、并发、数据库逐步推进到两个项目实战。

## 环境准备

本项目使用精确导出的 conda 环境配置。请在 Windows `cmd.exe` 中执行以下命令：

```bat
conda env create -f environment.yml
conda activate py311
```

如果本机已经存在 `py311` 环境，并希望按当前配置更新：

```bat
conda env update -f environment.yml --prune
conda activate py311
```

VS Code 已配置默认解释器路径为 `.conda\envs\py311\python.exe`，打开项目后确认右下角解释器为 `py311` 即可。若在 PowerShell 中无法执行 `conda activate`，请切换到 Windows CMD。

## 学习顺序

推荐按 [Python学习路线图.md](./Python学习路线图.md) 的章节顺序学习：

1. `01_基础篇`：入门、语法、控制流、字符串、容器
2. `02_函数篇`：函数基础、函数进阶
3. `03_模块与面向对象篇`：模块、包、OOP、异常
4. `04_高级特性篇`：文件、迭代器、生成器、装饰器
5. `05_标准库篇`：系统、文本、时间、数据结构、正则、网络与HTTP、数值工具、文件目录、类型注解、二进制处理、哈希摘要、枚举、UUID、ctypes、性能分析、内存管理、日志
6. `06_并发与数据库`：并发编程、SQLite
7. `07_项目实战`：代码组织、文件批量处理工具、日志分析系统

完成 `01_基础篇` 后，建议阅读 [Python基础补充.md](./Python基础补充.md)，了解包管理最小知识和 Python 常见坑。每章建议先通读源码注释，再运行示例，最后完成对应练习。

## 运行示例

在项目根目录执行单个章节：

```bat
python "01_基础篇\chapter01_入门与环境.py"
python "02_函数篇\chapter06_函数基础.py"
python "05_标准库篇\chapter21_补充01_网络与HTTP基础.py"
python "05_标准库篇\chapter21_补充02_数学与数值工具.py"
python "05_标准库篇\chapter21_补充03_文件目录工具.py"
python "05_标准库篇\chapter21_补充04_类型注解进阶.py"
python "05_标准库篇\chapter21_补充05_struct二进制数据处理.py"
python "05_标准库篇\chapter21_补充06_hashlib哈希与摘要.py"
python "05_标准库篇\chapter21_补充07_bytes与bytearray深入.py"
python "05_标准库篇\chapter21_补充08_enum枚举.py"
python "05_标准库篇\chapter21_补充09_uuid唯一标识符.py"
python "05_标准库篇\chapter21_补充10_ctypes调用C动态库.py"
python "05_标准库篇\chapter21_补充11_性能分析与优化.py"
python "05_标准库篇\chapter21_补充12_内存管理工具.py"
python "06_并发与数据库\chapter23_数据库操作.py"
```

运行练习题：

```bat
python "练习题与答案\exercises_01_基础.py"
python "练习题与答案\exercises_02_函数.py"
python "练习题与答案\exercises_03_面向对象.py"
python "练习题与答案\exercises_04_综合.py"
```

运行项目实战：

```bat
python "07_项目实战\chapter25_项目1_文件批量处理工具\main.py" --help
python "07_项目实战\chapter26_项目2_日志分析系统\main.py" --help
```

两个项目目录内也各有独立 `README.md`，可以按项目说明运行完整流程。

## 自动化测试

本项目使用标准库 `unittest`，不依赖第三方测试框架。

```bat
python -m unittest discover -s tests
```

当前测试主要覆盖：

- **章节烟雾测试**：ch01~ch23 全部主章节 + 12 个补充章节（网络、数值、文件目录、类型注解、struct、hashlib、bytes、enum、uuid、ctypes、性能分析、内存管理）的 main() 完整运行
- **基础练习**：Kaprekar 常数、数字提取、杨辉三角、购物车结算
- **函数练习**：pipe/compose、TTL 缓存装饰器、参数类型验证
- **面向对象练习**：银行账户（储蓄/信用）、数据类（Point/Rectangle/Color）、EventEmitter
- **综合练习**：日志文件解析与分析、CSV 分组聚合管道、SQLite 任务队列生命周期
- **项目集成测试**：文件批量处理工具 CLI、日志分析系统完整流程

这些测试用于验证学习工程的关键示例能正常运行，不替代每章的手动练习。

## 目录说明

```text
.
├── environment.yml
├── .gitignore
├── README.md
├── Python学习路线图.md
├── Python基础补充.md
├── main.py
├── 01_基础篇/
├── 02_函数篇/
├── 03_模块与面向对象篇/
├── 04_高级特性篇/
├── 05_标准库篇/
├── 06_并发与数据库/
├── 07_项目实战/
├── tests/
└── 练习题与答案/
```
