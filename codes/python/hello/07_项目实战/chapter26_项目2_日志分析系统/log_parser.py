"""
日志解析模块
负责将原始日志文本解析为结构化数据

知识点：
- re 模块：正则表达式解析日志行
- datetime：时间戳格式转换
- 生成器：逐行读取大文件，节省内存
- 多线程：并发解析多个文件
- dataclass：结构化日志记录
"""

import re
import logging
import threading
from datetime import datetime
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Optional, Iterator
from concurrent.futures import ThreadPoolExecutor, as_completed

logger = logging.getLogger(__name__)


# ------------------------------------------------------------------ #
#  数据类：日志记录                                                     #
# ------------------------------------------------------------------ #

@dataclass
class LogRecord:
    """
    解析后的单条日志记录

    使用 dataclass 自动生成 __init__、__repr__ 等方法
    """
    source_file: str                   # 来源文件
    ip: str                            # 客户端 IP
    timestamp: str                     # 时间（标准化为 YYYY-MM-DD HH:MM:SS）
    method: Optional[str] = None       # HTTP 方法
    url: Optional[str] = None          # 请求 URL（不含查询串）
    protocol: Optional[str] = None     # HTTP 协议版本
    status_code: Optional[int] = None  # HTTP 状态码
    bytes_sent: Optional[int] = None   # 发送字节数
    response_time: Optional[float] = None  # 响应时间（秒）
    user_agent: Optional[str] = None   # User-Agent
    referer: Optional[str] = None      # Referer

    def to_dict(self) -> dict:
        """转为字典（方便插入数据库）"""
        return asdict(self)


# ------------------------------------------------------------------ #
#  日志格式定义                                                         #
# ------------------------------------------------------------------ #

# Apache Combined Log Format:
# 127.0.0.1 - frank [10/Oct/2000:13:55:36 -0700] "GET /apache_pb.gif HTTP/1.0" 200 2326 0.003
APACHE_PATTERN = re.compile(
    r'(?P<ip>\S+)'           # IP 地址
    r'\s+\S+'                # ident（通常为 -）
    r'\s+\S+'                # authuser（通常为 -）
    r'\s+\[(?P<time>[^\]]+)\]'       # [时间]
    r'\s+"(?P<request>[^"]*)"'       # "请求行"
    r'\s+(?P<status>\d{3}|-)'        # 状态码
    r'\s+(?P<bytes>\d+|-)'           # 字节数
    r'(?:\s+(?P<resp_time>[\d.]+))?'  # 响应时间（可选）
)

# Nginx Combined Log Format（含 Referer 和 User-Agent）:
# 127.0.0.1 - - [01/Jan/2024:12:00:00 +0800] "GET / HTTP/1.1" 200 612 "-" "curl/7.68" 0.005
NGINX_PATTERN = re.compile(
    r'(?P<ip>\S+)'
    r'\s+\S+'
    r'\s+\S+'
    r'\s+\[(?P<time>[^\]]+)\]'
    r'\s+"(?P<request>[^"]*)"'
    r'\s+(?P<status>\d{3}|-)'
    r'\s+(?P<bytes>\d+|-)'
    r'\s+"(?P<referer>[^"]*)"'
    r'\s+"(?P<ua>[^"]*)"'
    r'(?:\s+(?P<resp_time>[\d.]+))?'
)

# Apache 日志时间格式
LOG_TIME_FORMAT = '%d/%b/%Y:%H:%M:%S %z'


def _parse_time(raw: str) -> str:
    """
    将日志时间字符串解析并转换为标准格式

    Args:
        raw: 原始时间字符串，如 '10/Oct/2000:13:55:36 -0700'

    Returns:
        标准时间字符串，如 '2000-10-10 13:55:36'
    """
    try:
        dt = datetime.strptime(raw.strip(), LOG_TIME_FORMAT)
        return dt.strftime('%Y-%m-%d %H:%M:%S')
    except ValueError:
        # 解析失败则原样返回
        return raw.strip()


def _parse_request(request_str: str) -> tuple[Optional[str], Optional[str], Optional[str]]:
    """
    解析请求行，提取方法、URL 和协议

    Args:
        request_str: 请求行，如 'GET /index.html HTTP/1.1'

    Returns:
        (method, url, protocol) 或 (None, None, None)
    """
    if not request_str or request_str == '-':
        return None, None, None

    parts = request_str.split(' ', 2)
    if len(parts) < 2:
        return None, request_str, None

    method = parts[0]
    full_url = parts[1]
    protocol = parts[2] if len(parts) > 2 else None

    # 只保留路径，去掉查询字符串
    url = full_url.split('?')[0] if full_url else full_url

    return method, url, protocol


def _safe_int(value: str, default: Optional[int] = None) -> Optional[int]:
    """安全转换整数，失败返回默认值"""
    try:
        return int(value) if value and value != '-' else default
    except (ValueError, TypeError):
        return default


def _safe_float(value: str, default: Optional[float] = None) -> Optional[float]:
    """安全转换浮点数，失败返回默认值"""
    try:
        return float(value) if value and value != '-' else default
    except (ValueError, TypeError):
        return default


# ------------------------------------------------------------------ #
#  解析函数                                                             #
# ------------------------------------------------------------------ #

def parse_apache_line(line: str, source_file: str) -> Optional[LogRecord]:
    """
    解析单行 Apache 格式日志

    Args:
        line: 原始日志行
        source_file: 来源文件名

    Returns:
        解析成功返回 LogRecord，失败返回 None
    """
    line = line.strip()
    if not line:
        return None

    m = APACHE_PATTERN.match(line)
    if not m:
        return None

    method, url, protocol = _parse_request(m.group('request'))

    return LogRecord(
        source_file=source_file,
        ip=m.group('ip'),
        timestamp=_parse_time(m.group('time')),
        method=method,
        url=url,
        protocol=protocol,
        status_code=_safe_int(m.group('status')),
        bytes_sent=_safe_int(m.group('bytes')),
        response_time=_safe_float(m.group('resp_time')),
        user_agent=None,
        referer=None,
    )


def parse_nginx_line(line: str, source_file: str) -> Optional[LogRecord]:
    """
    解析单行 Nginx 格式日志

    Args:
        line: 原始日志行
        source_file: 来源文件名

    Returns:
        解析成功返回 LogRecord，失败返回 None
    """
    line = line.strip()
    if not line:
        return None

    m = NGINX_PATTERN.match(line)
    if not m:
        return None

    method, url, protocol = _parse_request(m.group('request'))
    referer = m.group('referer')
    ua = m.group('ua')

    return LogRecord(
        source_file=source_file,
        ip=m.group('ip'),
        timestamp=_parse_time(m.group('time')),
        method=method,
        url=url,
        protocol=protocol,
        status_code=_safe_int(m.group('status')),
        bytes_sent=_safe_int(m.group('bytes')),
        response_time=_safe_float(m.group('resp_time')),
        user_agent=ua if ua != '-' else None,
        referer=referer if referer != '-' else None,
    )


# 格式名称到解析函数的映射
PARSERS = {
    'apache': parse_apache_line,
    'nginx': parse_nginx_line,
    # 'combined' 与 nginx 格式相同
    'combined': parse_nginx_line,
}


# ------------------------------------------------------------------ #
#  文件解析器                                                           #
# ------------------------------------------------------------------ #

class LogParser:
    """
    日志文件解析器

    支持：
    - 逐行生成器解析（内存友好）
    - 批量解析并返回记录列表
    - 多线程并发解析多个文件
    """

    def __init__(self, log_format: str = 'apache', batch_size: int = 1000):
        """
        初始化解析器

        Args:
            log_format: 日志格式，支持 'apache'/'nginx'/'combined'
            batch_size: 每批次处理的行数
        """
        if log_format not in PARSERS:
            raise ValueError(
                f"不支持的日志格式: {log_format}，"
                f"可用格式: {list(PARSERS.keys())}"
            )
        self.log_format = log_format
        self.batch_size = batch_size
        self._parse_line = PARSERS[log_format]

        # 统计计数器（使用锁保证线程安全）
        self._lock = threading.Lock()
        self.total_lines = 0
        self.parsed_lines = 0
        self.failed_lines = 0

    def _reset_stats(self) -> None:
        """重置统计计数器"""
        with self._lock:
            self.total_lines = 0
            self.parsed_lines = 0
            self.failed_lines = 0

    def _update_stats(self, total: int, parsed: int, failed: int) -> None:
        """线程安全地更新统计数据"""
        with self._lock:
            self.total_lines += total
            self.parsed_lines += parsed
            self.failed_lines += failed

    def iter_records(self, file_path: str) -> Iterator[LogRecord]:
        """
        逐行解析日志文件，以生成器方式返回记录
        （内存友好：不会一次性加载整个文件）

        Args:
            file_path: 日志文件路径

        Yields:
            每条成功解析的 LogRecord
        """
        path = Path(file_path)
        if not path.exists():
            raise FileNotFoundError(f"日志文件不存在: {file_path}")

        source_name = path.name
        total = parsed = failed = 0

        logger.info(f"开始解析文件: {file_path} (格式: {self.log_format})")

        with open(file_path, 'r', encoding='utf-8', errors='replace') as f:
            for line in f:
                total += 1
                record = self._parse_line(line, source_name)
                if record:
                    parsed += 1
                    yield record
                else:
                    failed += 1
                    if failed <= 5:
                        # 只记录前5条失败行，避免日志泛滥
                        logger.debug(f"解析失败 (第{total}行): {line.strip()[:80]}")

        self._update_stats(total, parsed, failed)
        logger.info(
            f"文件解析完成: {file_path} "
            f"(总计={total}, 成功={parsed}, 失败={failed})"
        )

    def parse_file(self, file_path: str) -> list[dict]:
        """
        解析单个日志文件，返回所有记录的字典列表

        Args:
            file_path: 日志文件路径

        Returns:
            记录字典列表（可直接用于数据库插入）
        """
        records = []
        for record in self.iter_records(file_path):
            records.append(record.to_dict())
        return records

    def parse_file_in_batches(
        self,
        file_path: str,
        callback  # Callable[[list[dict]], None]
    ) -> tuple[int, int, int]:
        """
        分批解析日志文件，每批完成后调用 callback 处理

        适合大文件：一批解析完立即处理，不积压内存

        Args:
            file_path: 日志文件路径
            callback: 接收批量记录列表的回调函数

        Returns:
            (total_lines, parsed_lines, failed_lines)
        """
        batch: list[dict] = []
        batch_count = 0

        for record in self.iter_records(file_path):
            batch.append(record.to_dict())
            if len(batch) >= self.batch_size:
                callback(batch)
                batch_count += 1
                logger.debug(f"已处理第 {batch_count} 批 ({self.batch_size} 条)")
                batch = []

        # 处理最后一批不足 batch_size 的记录
        if batch:
            callback(batch)
            batch_count += 1

        return self.total_lines, self.parsed_lines, self.failed_lines

    def parse_files_concurrent(
        self,
        file_paths: list[str],
        callback,         # Callable[[list[dict]], None]
        max_workers: int = 4
    ) -> dict:
        """
        多线程并发解析多个日志文件

        每个文件在独立线程中解析，解析结果通过 callback 回调处理。
        callback 需要是线程安全的（例如使用锁保护数据库写入）。

        Args:
            file_paths: 日志文件路径列表
            callback: 批量记录处理回调（需线程安全）
            max_workers: 最大并发线程数

        Returns:
            {'total': N, 'parsed': N, 'failed': N, 'files': {file: stats}}
        """
        self._reset_stats()
        file_stats: dict[str, dict] = {}
        results_lock = threading.Lock()

        def parse_one(file_path: str) -> tuple[str, int, int, int]:
            """在单线程中解析一个文件"""
            # 每个线程使用独立的解析器实例（避免计数冲突）
            parser = LogParser(self.log_format, self.batch_size)
            try:
                parser.parse_file_in_batches(file_path, callback)
                return (file_path, parser.total_lines,
                        parser.parsed_lines, parser.failed_lines)
            except Exception as e:
                logger.error(f"解析文件 {file_path} 时出错: {e}")
                return file_path, 0, 0, 0

        logger.info(
            f"开始并发解析 {len(file_paths)} 个文件 "
            f"(线程数={max_workers})"
        )

        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = {
                executor.submit(parse_one, fp): fp
                for fp in file_paths
            }

            for future in as_completed(futures):
                file_path, total, parsed, failed = future.result()
                with results_lock:
                    file_stats[file_path] = {
                        'total': total,
                        'parsed': parsed,
                        'failed': failed,
                    }
                    self.total_lines += total
                    self.parsed_lines += parsed
                    self.failed_lines += failed

        summary = {
            'total': self.total_lines,
            'parsed': self.parsed_lines,
            'failed': self.failed_lines,
            'files': file_stats,
        }
        logger.info(
            f"并发解析完成: "
            f"总计={self.total_lines}, "
            f"成功={self.parsed_lines}, "
            f"失败={self.failed_lines}"
        )
        return summary

    @property
    def stats(self) -> dict:
        """返回当前解析统计信息"""
        with self._lock:
            return {
                'total_lines': self.total_lines,
                'parsed_lines': self.parsed_lines,
                'failed_lines': self.failed_lines,
                'success_rate': (
                    f"{self.parsed_lines / self.total_lines:.1%}"
                    if self.total_lines > 0 else 'N/A'
                ),
            }


# ------------------------------------------------------------------ #
#  示例用法                                                             #
# ------------------------------------------------------------------ #

if __name__ == '__main__':
    import os
    import tempfile

    logging.basicConfig(level=logging.INFO)

    # 生成测试日志文件
    sample_apache_logs = """\
192.168.1.1 - - [01/Jan/2024:10:00:01 +0800] "GET /index.html HTTP/1.1" 200 1024 0.050
10.0.0.2 - - [01/Jan/2024:10:00:02 +0800] "POST /api/login HTTP/1.1" 200 256 0.120
172.16.0.3 - - [01/Jan/2024:10:00:03 +0800] "GET /images/logo.png HTTP/1.1" 200 8192 0.030
192.168.1.1 - - [01/Jan/2024:10:01:00 +0800] "GET /about.html HTTP/1.1" 404 512 0.010
10.0.0.5 - - [01/Jan/2024:10:02:30 +0800] "GET /api/data HTTP/1.1" 500 128 1.500
invalid log line without proper format
192.168.1.2 - - [01/Jan/2024:11:00:00 +0800] "DELETE /admin HTTP/1.1" 403 64 0.005
"""

    with tempfile.NamedTemporaryFile(
        mode='w', suffix='.log', delete=False, encoding='utf-8'
    ) as f:
        f.write(sample_apache_logs)
        tmp_log = f.name

    try:
        print("=== 测试日志解析器 ===\n")

        parser = LogParser(log_format='apache', batch_size=3)

        # 1. 生成器方式逐条解析
        print("--- 逐条解析 ---")
        for record in parser.iter_records(tmp_log):
            print(f"  [{record.status_code}] {record.method} {record.url} "
                  f"来自 {record.ip} 耗时 {record.response_time}s")

        print(f"\n解析统计: {parser.stats}\n")

        # 2. 分批解析
        print("--- 分批解析（每批3条） ---")
        batch_results: list[dict] = []

        def on_batch(batch: list[dict]) -> None:
            batch_results.extend(batch)
            print(f"  收到一批: {len(batch)} 条记录")

        parser._reset_stats()
        parser.parse_file_in_batches(tmp_log, on_batch)
        print(f"共收到 {len(batch_results)} 条记录")
        print(f"解析统计: {parser.stats}\n")

    finally:
        os.unlink(tmp_log)
