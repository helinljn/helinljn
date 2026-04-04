# 第26章：综合项目2 - 日志分析系统

## 项目概述

这是一个功能完整的日志分析系统，用于解析、分析和统计各类日志文件。项目综合运用了前面学习的多个知识点。

## 功能特性

1. **日志解析**
   - 支持多种日志格式（Apache、Nginx、自定义格式）
   - 正则表达式模式匹配
   - 时间戳解析

2. **数据分析**
   - 访问统计（PV、UV）
   - 状态码分布
   - 响应时间分析
   - IP 访问排名
   - URL 访问排名
   - 时间段分布

3. **数据存储**
   - SQLite 数据库持久化
   - 支持增量分析
   - 历史数据查询

4. **报告生成**
   - JSON 格式报告
   - CSV 格式报告
   - 控制台输出

5. **性能优化**
   - 多线程并发处理
   - 批量数据库操作
   - 内存优化

## 技术要点

- **正则表达式**：日志格式解析
- **datetime**：时间处理
- **sqlite3**：数据存储
- **threading**：多线程处理
- **collections**：数据统计
- **json/csv**：报告生成
- **logging**：日志记录
- **argparse**：命令行参数

## 项目结构

```
chapter26_项目2_日志分析系统/
├── main.py              # 主程序入口
├── log_parser.py        # 日志解析模块
├── analyzer.py          # 数据分析模块
├── reporter.py          # 报告生成模块
├── database.py          # 数据库操作模块
├── config.py            # 配置管理
├── config.ini           # 配置文件
├── sample_logs/         # 示例日志文件
│   ├── access.log
│   └── nginx.log
└── README.md            # 本文件
```

## 使用方法

### 基本用法

```bash
# 分析单个日志文件
python main.py -f sample_logs/access.log

# 分析多个日志文件
python main.py -f sample_logs/access.log sample_logs/nginx.log

# 指定日志格式
python main.py -f access.log --format apache

# 生成 JSON 报告
python main.py -f access.log -o report.json --format json

# 生成 CSV 报告
python main.py -f access.log -o report.csv --format csv

# 使用多线程（4个线程）
python main.py -f access.log --threads 4

# 查看历史统计
python main.py --history

# 清空数据库
python main.py --clear-db
```

### 配置文件

编辑 `config.ini` 自定义配置：

```ini
[database]
db_file = logs.db

[parser]
default_format = apache
batch_size = 1000

[analyzer]
top_n = 10

[reporter]
default_format = console
```

## 示例日志格式

### Apache 格式
```
127.0.0.1 - - [01/Jan/2024:12:00:00 +0800] "GET /index.html HTTP/1.1" 200 1234 0.123
```

### Nginx 格式
```
127.0.0.1 - - [01/Jan/2024:12:00:00 +0800] "GET /index.html HTTP/1.1" 200 1234 "-" "Mozilla/5.0" 0.123
```

## 分析报告示例

```
=== 日志分析报告 ===
分析时间: 2024-01-01 12:00:00
日志文件: access.log
总记录数: 10000

--- 基本统计 ---
总访问量(PV): 10000
独立访客(UV): 1234
平均响应时间: 0.156s
最大响应时间: 2.345s

--- 状态码分布 ---
200: 8500 (85.0%)
404: 1000 (10.0%)
500: 500 (5.0%)

--- TOP 10 访问IP ---
1. 192.168.1.100: 500次
2. 192.168.1.101: 450次
...

--- TOP 10 访问URL ---
1. /index.html: 2000次
2. /api/data: 1500次
...
```

## 学习要点

1. **模块化设计**：功能分离，职责清晰
2. **配置管理**：使用配置文件，提高灵活性
3. **异常处理**：完善的错误处理机制
4. **性能优化**：多线程、批量操作
5. **代码规范**：遵循 PEP 8，添加类型注解
6. **文档完善**：详细的注释和文档字符串

## 扩展建议

1. 支持更多日志格式
2. 添加实时监控功能
3. 实现 Web 可视化界面
4. 支持分布式日志收集
5. 添加告警功能
6. 支持日志压缩文件
7. 实现增量分析优化

## 注意事项

1. 大文件处理时注意内存使用
2. 数据库操作使用事务
3. 多线程注意线程安全
4. 正则表达式性能优化
5. 异常情况的处理
