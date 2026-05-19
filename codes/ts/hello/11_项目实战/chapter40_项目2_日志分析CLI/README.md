# 项目 2：日志分析工具

这个项目演示 Node 文件读取、解析、聚合和报告输出。日志格式采用简化版 HTTP access log。

```bat
npm run build
node dist/11_项目实战/chapter40_项目2_日志分析CLI/main.js --log 11_项目实战/chapter40_项目2_日志分析CLI/sample_logs/access.log --format text --top 3
```

输出格式：

- `text`：面向终端阅读
- `json`：便于后续程序处理
