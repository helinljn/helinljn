# 第 25 章：综合项目 1 - 文件批量处理工具

## 项目简介

这是一个功能完整的命令行文件批量处理工具，整合了前面学习的多个知识点：
- 文件操作（pathlib, os）
- 命令行参数解析（argparse）
- 日志记录（logging）
- 配置文件（configparser）
- 正则表达式（re）
- 异常处理
- 面向对象设计

## 功能特性

1. **文件搜索与过滤**
   - 按文件名模式搜索
   - 按文件大小过滤
   - 按修改时间过滤
   - 递归搜索子目录

2. **批量重命名**
   - 添加前缀/后缀
   - 替换文件名中的文本
   - 使用正则表达式重命名
   - 序号命名

3. **文件内容替换**
   - 文本文件内容批量替换
   - 支持正则表达式
   - 备份原文件

4. **日志记录**
   - 详细的操作日志
   - 错误追踪
   - 日志文件轮转

5. **配置管理**
   - 配置文件支持
   - 命令行参数覆盖配置

## 项目结构

```
chapter25_项目1_文件批量处理工具/
├── main.py              # 程序入口
├── file_processor.py    # 核心处理类
├── config.py            # 配置管理
├── utils.py             # 工具函数
├── config.ini           # 配置文件示例
└── README.md            # 本文件
```

## 使用示例

### 1. 搜索文件
```bash
python main.py search --pattern "*.txt" --path ./test_files
```

### 2. 批量重命名（添加前缀）
```bash
python main.py rename --pattern "*.jpg" --prefix "photo_" --path ./images
```

### 3. 批量重命名（序号）
```bash
python main.py rename --pattern "*.mp3" --numbered --path ./music
```

### 4. 文件内容替换
```bash
python main.py replace --pattern "*.py" --find "old_name" --replace "new_name" --path ./src
```

### 5. 使用正则表达式重命名
```bash
python main.py rename --pattern "*.txt" --regex "(\d+)" --replace "file_\1" --path ./docs
```

## 配置文件

创建 `config.ini` 文件：

```ini
[DEFAULT]
log_level = INFO
log_file = file_processor.log
backup_enabled = true

[SEARCH]
recursive = true
ignore_hidden = true

[RENAME]
dry_run = false
create_backup = true
```

## 学习要点

1. **项目架构设计**
   - 模块化设计
   - 职责分离
   - 可扩展性

2. **错误处理**
   - 完善的异常捕获
   - 用户友好的错误提示
   - 日志记录

3. **代码规范**
   - PEP 8 规范
   - 类型注解
   - 文档字符串

4. **实用技巧**
   - 命令行工具开发
   - 配置管理
   - 日志系统

## 扩展建议

1. 添加更多文件操作功能（复制、移动、删除）
2. 支持更多文件格式
3. 添加 GUI 界面
4. 支持撤销操作
5. 添加进度条显示
6. 支持多线程处理大量文件
