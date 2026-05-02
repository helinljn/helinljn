# =============================================================================
# 综合练习 04：综合篇
# 覆盖：ch12 文件操作、ch13 迭代器与生成器、ch14 装饰器详解、
#       ch15-21 标准库、ch22 并发编程、ch23 数据库操作
# =============================================================================

import json
import csv
import sqlite3
import time
import os
import sys
import re
from pathlib import Path
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor, as_completed
from functools import wraps
from typing import Iterator, Generator, Any


# =============================================================================
# 练习 1：日志文件解析器（生成器 + 文件操作）
# =============================================================================
# 解析 Apache/Nginx 风格的日志文件，逐行读取并提取结构化信息。
#
# 要求：
#   - 使用生成器 parse_log_lines(path: str) -> Generator[dict, None, None]
#     逐行 yield 解析后的字典，不一次性加载整个文件
#   - 使用正则表达式提取：IP、时间、方法、路径、状态码、响应大小
#   - 如果文件不存在，捕获异常并打印友好提示
#   - 示例日志行：
#     192.168.1.1 - - [01/Jan/2024:13:00:00 +0000] "GET /index.html HTTP/1.1" 200 2326


# =============================================================================
# 练习 2：CSV 数据处理管道
# =============================================================================
# 构建一个数据处理管道，读入 CSV 文件，进行清洗、转换、聚合。
#
# 要求：
#   - 读取 CSV 文件（使用 csv.DictReader）
#   - 过滤掉无效行（空值、格式错误的数值）
#   - 按指定列进行分组并计算聚合值（总和、平均、计数）
#   - 将结果保存为 JSON 文件
#   - 使用生成器使管道惰性处理（不一次性加载全部数据到内存）


# =============================================================================
# 练习 3：并发文件搜索工具
# =============================================================================
# 实现一个多线程文件搜索工具。
#
# 要求：
#   - 使用 ThreadPoolExecutor 并行搜索多个目录
#   - 支持按文件名模式（glob）和文件内容（正则）搜索
#   - 使用 Queue 收集搜索结果
#   - 可设置超时时间
#   - 返回结果按修改时间排序


# =============================================================================
# 练习 4：SQLite 任务队列
# =============================================================================
# 使用 SQLite 实现一个简单的任务队列。
#
# 要求：
#   - 表结构：id, task_name, payload(JSON), status, created_at, completed_at
#   - 方法：enqueue(name, data), dequeue() → dict, complete(task_id)
#   - 事务处理：enqueue 和 dequeue 使用事务保证一致性
#   - 使用参数化查询防止 SQL 注入
#   - 状态流转：pending → processing → completed/failed


# =============================================================================
# 练习 5：简单的性能分析装饰器
# =============================================================================
# 实现一个功能完整的性能分析装饰器。
#
# 要求：
#   - @profile(log_file=None, min_time=0) 装饰器
#   - 记录每次调用的执行时间和参数
#   - 如果指定了 log_file，将结果写入日志文件
#   - 追踪调用次数和平均执行时间
#   - 如果执行时间超过 min_time，打印警告
#   - 使用 time.perf_counter() 获取高精度时间


# =============================================================================
# ============================= 答案分隔线 ====================================
# =============================================================================


# =============================================================================
# 练习 1 答案：日志文件解析器
# =============================================================================

# Apache 日志正则（组合日志格式）
LOG_PATTERN = re.compile(
    r'(?P<ip>\S+)'                           # IP 地址
    r'\s+\S+\s+\S+'                          # 两个标识符（- -）
    r'\s+\[(?P<time>[^\]]+)\]'               # 时间戳 [01/Jan/2024:...]
    r'\s+"(?P<method>\S+)'                   # HTTP 方法 GET/POST/...
    r'\s+(?P<path>\S+)'                      # 请求路径
    r'\s+\S+"'                               # HTTP 版本
    r'\s+(?P<status>\d{3})'                  # 状态码
    r'\s+(?P<size>\d+|-)'                    # 响应大小
)


def parse_log_lines(path: str) -> Generator[dict, None, None]:
    """逐行解析日志文件，返回结构化的字典生成器。"""
    file_path = Path(path)
    if not file_path.exists():
        print(f"⚠️  文件不存在: {path}")
        return

    with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue

            match = LOG_PATTERN.match(line)
            if match:
                data = match.groupdict()
                data["line"] = line_num
                # 将 size 转为 int（"-" 表示请求未完成）
                try:
                    data["size"] = int(data["size"])
                except ValueError:
                    data["size"] = 0
                data["status"] = int(data["status"])
                yield data
            else:
                # 无法解析的行，返回原始内容
                yield {"line": line_num, "raw": line, "parse_error": True}


def analyze_log_file(path: str) -> dict:
    """分析日志文件并返回统计报告。"""
    if not Path(path).exists():
        return {"error": f"文件不存在: {path}"}

    status_counts: Counter[int] = Counter()
    ip_counts: Counter[str] = Counter()
    total_bytes = 0
    total_requests = 0
    methods: Counter[str] = Counter()
    paths: Counter[str] = Counter()

    for entry in parse_log_lines(path):
        if entry.get("parse_error"):
            continue

        total_requests += 1
        status_counts[entry["status"]] += 1
        ip_counts[entry["ip"]] += 1
        total_bytes += entry["size"]
        methods[entry["method"]] += 1
        paths[entry["path"]] += 1

    if total_requests == 0:
        return {"error": "没有有效的日志条目"}

    return {
        "total_requests": total_requests,
        "total_bytes": total_bytes,
        "avg_size": round(total_bytes / total_requests, 1),
        "status_distribution": dict(status_counts.most_common(10)),
        "top_ips": dict(ip_counts.most_common(5)),
        "method_distribution": dict(methods),
        "top_paths": dict(paths.most_common(5)),
    }


# =============================================================================
# 练习 2 答案：CSV 数据处理管道
# =============================================================================

def read_csv_rows(path: str) -> Generator[dict, None, None]:
    """惰性读取 CSV 文件，逐行 yield。"""
    file_path = Path(path)
    if not file_path.exists():
        print(f"⚠️  文件不存在: {path}")
        return

    with open(file_path, "r", encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            yield row


def filter_valid_rows(rows: Generator[dict, None, None],
                      required_fields: list[str],
                      numeric_fields: list[str]) -> Generator[dict, None, None]:
    """过滤无效行（缺少必需字段或数值字段格式错误）。"""
    for row in rows:
        # 检查必需字段
        if any(not row.get(f) for f in required_fields):
            continue
        # 检查数值字段
        try:
            for f in numeric_fields:
                float(row.get(f, "0"))
        except (ValueError, TypeError):
            continue
        yield row


def group_and_aggregate(rows: Generator[dict, None, None],
                        group_by: str,
                        aggregate_field: str) -> dict:
    """按指定列分组并计算聚合值。"""
    groups: dict[str, list[float]] = defaultdict(list)

    for row in rows:
        key = row[group_by]
        value = float(row[aggregate_field])
        groups[key].append(value)

    result = {}
    for key, values in groups.items():
        result[key] = {
            "count": len(values),
            "sum": round(sum(values), 2),
            "avg": round(sum(values) / len(values), 2),
            "min": min(values),
            "max": max(values),
        }
    return result


def csv_pipeline(input_path: str, output_path: str, group_by: str,
                 aggregate_field: str, required_fields: list[str],
                 numeric_fields: list[str]) -> dict:
    """完整的 CSV 数据处理管道。"""
    # 管道：读取 → 过滤 → 分组聚合
    rows = read_csv_rows(input_path)
    valid_rows = filter_valid_rows(rows, required_fields, numeric_fields)
    results = group_and_aggregate(valid_rows, group_by, aggregate_field)

    # 保存结果到 JSON
    output_data = {
        "source": input_path,
        "group_by": group_by,
        "aggregate_field": aggregate_field,
        "results": results,
    }
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(output_data, f, ensure_ascii=False, indent=2)

    return output_data


# =============================================================================
# 练习 3 答案：并发文件搜索工具
# =============================================================================

from queue import Queue


def search_file_content(file_path: str, pattern: str) -> list[dict]:
    """搜索单个文件的内容。"""
    matches = []
    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            for line_num, line in enumerate(f, 1):
                if re.search(pattern, line):
                    matches.append({
                        "file": file_path,
                        "line": line_num,
                        "content": line.strip()[:200],  # 截断长行
                    })
    except (OSError, PermissionError):
        pass
    return matches


def search_directories(directories: list[str], name_pattern: str = "*",
                       content_pattern: str = "", max_workers: int = 4,
                       timeout: float = 30.0) -> list[dict]:
    """并发搜索多个目录中的文件。"""
    # 收集所有匹配文件名模式的文件
    all_files: list[str] = []
    for directory in directories:
        dir_path = Path(directory)
        if not dir_path.exists():
            print(f"⚠️  目录不存在: {directory}")
            continue
        for file_path in dir_path.rglob(name_pattern):
            if file_path.is_file():
                all_files.append(str(file_path))

    if not all_files:
        print("没有找到匹配的文件")
        return []

    # 并发搜索文件内容
    results: list[dict] = []
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = {
            executor.submit(search_file_content, f, content_pattern): f
            for f in all_files
        }
        for future in as_completed(futures, timeout=timeout):
            try:
                matches = future.result()
                results.extend(matches)
            except Exception as e:
                file_path = futures[future]
                print(f"⚠️  搜索文件失败: {file_path}: {e}")

    # 按文件路径排序
    results.sort(key=lambda x: (x["file"], x["line"]))
    return results


# =============================================================================
# 练习 4 答案：SQLite 任务队列
# =============================================================================

class TaskQueue:
    """基于 SQLite 的简单任务队列。"""

    def __init__(self, db_path: str = ":memory:") -> None:
        self.conn = sqlite3.connect(db_path)
        self.conn.row_factory = sqlite3.Row
        self._create_table()

    def _create_table(self) -> None:
        """创建任务表。"""
        self.conn.execute("""
            CREATE TABLE IF NOT EXISTS tasks (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                task_name TEXT NOT NULL,
                payload TEXT NOT NULL DEFAULT '{}',
                status TEXT NOT NULL DEFAULT 'pending',
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                completed_at TIMESTAMP
            )
        """)
        self.conn.commit()

    def enqueue(self, task_name: str, data: dict | None = None) -> int:
        """添加任务到队列，返回任务 ID。"""
        payload = json.dumps(data or {})
        cursor = self.conn.execute(
            "INSERT INTO tasks (task_name, payload) VALUES (?, ?)",
            (task_name, payload),
        )
        self.conn.commit()
        return cursor.lastrowid  # type: ignore

    def dequeue(self) -> dict | None:
        """取出一个 pending 状态的任务并标记为 processing。
        使用事务保证原子性。"""
        with self.conn:
            cursor = self.conn.execute(
                "SELECT id FROM tasks WHERE status = 'pending' "
                "ORDER BY created_at LIMIT 1"
            )
            row = cursor.fetchone()
            if not row:
                return None

            task_id = row[0]
            self.conn.execute(
                "UPDATE tasks SET status = 'processing' WHERE id = ?",
                (task_id,),
            )

            cursor = self.conn.execute(
                "SELECT * FROM tasks WHERE id = ?", (task_id,)
            )
            task_row = cursor.fetchone()
            if task_row:
                task = dict(task_row)
                task["payload"] = json.loads(task["payload"])
                return task
        return None

    def complete(self, task_id: int, success: bool = True) -> None:
        """完成任务。"""
        status = "completed" if success else "failed"
        self.conn.execute(
            "UPDATE tasks SET status = ?, completed_at = CURRENT_TIMESTAMP "
            "WHERE id = ?",
            (status, task_id),
        )
        self.conn.commit()

    def stats(self) -> dict:
        """获取队列统计信息。"""
        cursor = self.conn.execute(
            "SELECT status, COUNT(*) as count FROM tasks GROUP BY status"
        )
        return {row["status"]: row["count"] for row in cursor.fetchall()}

    def close(self) -> None:
        """关闭数据库连接。"""
        self.conn.close()


# =============================================================================
# 练习 5 答案：性能分析装饰器
# =============================================================================

def profile(log_file: str | None = None, min_time: float = 0.0) -> Any:
    """性能分析装饰器。

    Args:
        log_file: 日志文件路径（None 表示只打印到控制台）
        min_time: 最短记录时间（秒），超过此时间才打印警告
    """

    def decorator(func):
        # 使用闭包记录统计信息
        stats = {"calls": 0, "total_time": 0.0}

        @wraps(func)
        def wrapper(*args, **kwargs):
            start = time.perf_counter()
            try:
                return func(*args, **kwargs)
            finally:
                elapsed = time.perf_counter() - start
                stats["calls"] += 1
                stats["total_time"] += elapsed
                avg_time = stats["total_time"] / stats["calls"]

                msg = (
                    f"[{func.__name__}] 耗时: {elapsed:.4f}s | "
                    f"调用: {stats['calls']}次 | 平均: {avg_time:.4f}s"
                )

                # 超时警告
                if elapsed > min_time > 0:
                    msg += f" ⚠️  超过阈值 {min_time}s"

                # 输出日志
                if log_file:
                    with open(log_file, "a", encoding="utf-8") as f:
                        f.write(
                            f"{time.strftime('%Y-%m-%d %H:%M:%S')} {msg}\n"
                        )
                print(f"  {msg}")

        # 暴露统计信息
        wrapper.stats = stats  # type: ignore
        return wrapper

    return decorator


# =============================================================================
# 运行所有练习
# =============================================================================

def create_sample_data() -> None:
    """为练习创建示例数据文件。"""
    sample_dir = Path(__file__).parent / "_sample_data"
    sample_dir.mkdir(exist_ok=True)

    # 示例日志文件
    log_content = """192.168.1.1 - - [01/Jan/2024:13:00:35 +0000] "GET /index.html HTTP/1.1" 200 2326
10.0.0.5 - - [01/Jan/2024:13:01:12 +0000] "POST /api/login HTTP/1.1" 200 1250
192.168.1.1 - - [01/Jan/2024:13:02:05 +0000] "GET /style.css HTTP/1.1" 304 0
10.0.0.5 - - [01/Jan/2024:13:03:28 +0000] "GET /about.html HTTP/1.1" 200 4500
172.16.0.1 - - [01/Jan/2024:13:04:17 +0000] "GET /api/users HTTP/1.1" 500 1230
192.168.1.1 - - [01/Jan/2024:13:05:42 +0000] "GET /favicon.ico HTTP/1.1" 404 45
10.0.0.5 - - [01/Jan/2024:13:06:10 +0000] "POST /api/search HTTP/1.1" 200 8920
172.16.0.1 - - [01/Jan/2024:13:07:55 +0000] "GET /contact.html HTTP/1.1" 200 3100
"""

    log_path = sample_dir / "access.log"
    log_path.write_text(log_content, encoding="utf-8")

    # 示例 CSV 文件
    csv_content = """name,category,price,quantity
商品A,电子产品,199.99,10
商品B,图书,29.50,50
商品C,电子产品,399.00,5
商品D,服装,89.00,30
商品E,图书,45.00,25
商品F,电子产品,149.50,15
商品G,服装,120.00,20
商品H,电子产品,599.00,3
"""
    csv_path = sample_dir / "products.csv"
    csv_path.write_text(csv_content, encoding="utf-8")


if __name__ == "__main__":
    print("=" * 60)
    print("综合练习 04：综合篇")
    print("=" * 60)

    create_sample_data()
    data_dir = Path(__file__).parent / "_sample_data"

    # 练习 1
    print("\n【练习 1】日志文件解析器")
    log_file = data_dir / "access.log"
    print(f"  解析日志文件: {log_file}")

    # 使用生成器逐行解析
    for entry in parse_log_lines(str(log_file)):
        if not entry.get("parse_error"):
            print(f"  [{entry['line']}] {entry['method']} {entry['path']} "
                  f"→ {entry['status']} ({entry['size']} bytes)")

    print("\n  日志分析报告:")
    report = analyze_log_file(str(log_file))
    if "error" not in report:
        print(f"    总请求: {report['total_requests']}")
        print(f"    总流量: {report['total_bytes']} bytes")
        print(f"    状态码分布: {report['status_distribution']}")
        print(f"    热门IP: {report['top_ips']}")

    # 练习 2
    print("\n【练习 2】CSV 数据处理管道")
    csv_file = data_dir / "products.csv"
    output_file = data_dir / "products_analysis.json"

    pipeline_result = csv_pipeline(
        str(csv_file), str(output_file),
        group_by="category", aggregate_field="price",
        required_fields=["name", "category", "price"],
        numeric_fields=["price", "quantity"],
    )
    for category, stats in pipeline_result["results"].items():
        print(f"  {category}: 数量={stats['count']}, "
              f"平均价格={stats['avg']}, 总价={stats['sum']}")

    # 练习 3
    print("\n【练习 3】并发文件搜索工具")
    search_results = search_directories(
        [str(data_dir)],
        name_pattern="*.log",
        content_pattern=r"20[0-9]",
        max_workers=2,
    )
    print(f"  找到 {len(search_results)} 个匹配")
    for m in search_results[:3]:
        print(f"  {m['file']}:{m['line']} → {m['content'][:80]}")

    # 练习 4
    print("\n【练习 4】SQLite 任务队列")
    queue = TaskQueue(":memory:")
    task_id = queue.enqueue("发送邮件", {"to": "user@example.com", "subject": "Hello"})
    queue.enqueue("生成报告", {"type": "monthly", "year": 2024})
    queue.enqueue("清理缓存", {"older_than": "30d"})
    print(f"  队列状态: {queue.stats()}")

    task = queue.dequeue()
    if task:
        print(f"  取出任务: [{task['id']}] {task['task_name']} → {task['payload']}")
        queue.complete(task["id"], success=True)

    task = queue.dequeue()
    if task:
        print(f"  取出任务: [{task['id']}] {task['task_name']} → {task['payload']}")
        queue.complete(task["id"], success=False)

    print(f"  最终状态: {queue.stats()}")
    queue.close()

    # 练习 5
    print("\n【练习 5】性能分析装饰器")

    @profile(min_time=0.01)
    def slow_calculation(n: int) -> int:
        """模拟一个耗时的计算函数。"""
        time.sleep(0.02)
        return sum(i * i for i in range(n * 1000))

    @profile(min_time=0.01)
    def fast_calculation(n: int) -> int:
        """模拟一个快速的计算函数。"""
        return n * 2

    _ = slow_calculation(5)
    _ = slow_calculation(3)
    _ = fast_calculation(10)

    # 查看统计信息
    print(f"\n  slow_calculation 统计: {slow_calculation.stats}")  # type: ignore
    print(f"  fast_calculation 统计: {fast_calculation.stats}")    # type: ignore

    # 清理示例数据
    import shutil
    shutil.rmtree(data_dir, ignore_errors=True)
