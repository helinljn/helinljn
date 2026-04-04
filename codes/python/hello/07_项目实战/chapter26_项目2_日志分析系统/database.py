"""
数据库操作模块
负责 SQLite 数据库的初始化、读写和查询操作

知识点：
- sqlite3 模块：数据库连接、游标操作
- 上下文管理器：with 语句管理连接
- 事务处理：批量操作的性能优化
- 线程安全：threading.local 隔离连接
"""

import sqlite3
import threading
import logging
from datetime import datetime
from typing import Optional

# 获取日志记录器
logger = logging.getLogger(__name__)

# 线程本地存储（每个线程维护自己的数据库连接，保证线程安全）
_thread_local = threading.local()


class Database:
    """
    数据库管理类

    使用线程本地存储确保多线程环境下的连接安全，
    每个线程拥有独立的连接，避免并发冲突。
    """

    def __init__(self, db_file: str = 'logs.db'):
        """
        初始化数据库

        Args:
            db_file: SQLite 数据库文件路径
        """
        self.db_file = db_file
        self._create_tables()

    def _get_connection(self) -> sqlite3.Connection:
        """
        获取当前线程的数据库连接（线程本地存储）

        Returns:
            当前线程的 SQLite 连接
        """
        if not hasattr(_thread_local, 'connection'):
            # 该线程尚无连接，创建一个新连接
            _thread_local.connection = sqlite3.connect(self.db_file)
            # 设置行工厂，让查询结果支持字典方式访问
            _thread_local.connection.row_factory = sqlite3.Row
            logger.debug(f"线程 {threading.current_thread().name} 创建新数据库连接")
        return _thread_local.connection

    @property
    def conn(self) -> sqlite3.Connection:
        """获取当前线程的连接（属性快捷访问）"""
        return self._get_connection()

    def _create_tables(self) -> None:
        """创建数据库表结构"""
        with self.conn:
            # 日志记录表：存储每条解析后的日志
            self.conn.execute('''
                CREATE TABLE IF NOT EXISTS log_records (
                    id          INTEGER PRIMARY KEY AUTOINCREMENT,
                    source_file TEXT NOT NULL,           -- 来源日志文件
                    ip          TEXT NOT NULL,           -- 客户端 IP
                    timestamp   TEXT NOT NULL,           -- 请求时间（字符串）
                    method      TEXT,                    -- HTTP 方法
                    url         TEXT,                    -- 请求 URL
                    protocol    TEXT,                    -- HTTP 协议版本
                    status_code INTEGER,                 -- HTTP 状态码
                    bytes_sent  INTEGER,                 -- 发送字节数
                    response_time REAL,                  -- 响应时间（秒）
                    user_agent  TEXT,                    -- User-Agent
                    referer     TEXT,                    -- Referer
                    created_at  TEXT DEFAULT (datetime('now'))  -- 入库时间
                )
            ''')

            # 分析任务表：记录每次分析的元数据
            self.conn.execute('''
                CREATE TABLE IF NOT EXISTS analysis_tasks (
                    id          INTEGER PRIMARY KEY AUTOINCREMENT,
                    source_file TEXT NOT NULL,           -- 分析的日志文件
                    log_format  TEXT NOT NULL,           -- 日志格式
                    total_lines INTEGER DEFAULT 0,       -- 总行数
                    parsed_lines INTEGER DEFAULT 0,      -- 成功解析行数
                    failed_lines INTEGER DEFAULT 0,      -- 解析失败行数
                    start_time  TEXT NOT NULL,           -- 分析开始时间
                    end_time    TEXT,                    -- 分析结束时间
                    status      TEXT DEFAULT 'running'   -- 状态: running/completed/failed
                )
            ''')

            # 为常用查询字段创建索引，提升查询性能
            self.conn.execute('''
                CREATE INDEX IF NOT EXISTS idx_log_ip
                ON log_records(ip)
            ''')
            self.conn.execute('''
                CREATE INDEX IF NOT EXISTS idx_log_url
                ON log_records(url)
            ''')
            self.conn.execute('''
                CREATE INDEX IF NOT EXISTS idx_log_status
                ON log_records(status_code)
            ''')
            self.conn.execute('''
                CREATE INDEX IF NOT EXISTS idx_log_source
                ON log_records(source_file)
            ''')
            self.conn.execute('''
                CREATE INDEX IF NOT EXISTS idx_log_timestamp
                ON log_records(timestamp)
            ''')

        logger.info(f"数据库表结构初始化完成: {self.db_file}")

    def insert_log_batch(self, records: list[dict]) -> int:
        """
        批量插入日志记录（使用事务，提升性能）

        Args:
            records: 日志记录列表，每条为字典

        Returns:
            成功插入的记录数
        """
        if not records:
            return 0

        sql = '''
            INSERT INTO log_records
                (source_file, ip, timestamp, method, url, protocol,
                 status_code, bytes_sent, response_time, user_agent, referer)
            VALUES
                (:source_file, :ip, :timestamp, :method, :url, :protocol,
                 :status_code, :bytes_sent, :response_time, :user_agent, :referer)
        '''
        try:
            # executemany 批量执行，在单个事务中完成
            with self.conn:
                self.conn.executemany(sql, records)
            logger.debug(f"批量插入 {len(records)} 条记录")
            return len(records)
        except sqlite3.Error as e:
            logger.error(f"批量插入失败: {e}")
            raise

    def create_task(self, source_file: str, log_format: str) -> int:
        """
        创建分析任务记录

        Args:
            source_file: 日志文件路径
            log_format: 日志格式

        Returns:
            任务 ID
        """
        sql = '''
            INSERT INTO analysis_tasks (source_file, log_format, start_time)
            VALUES (?, ?, ?)
        '''
        with self.conn:
            cursor = self.conn.execute(
                sql, (source_file, log_format, datetime.now().isoformat())
            )
        task_id = cursor.lastrowid
        logger.info(f"创建分析任务 #{task_id}: {source_file}")
        return task_id

    def update_task(
        self,
        task_id: int,
        total_lines: int,
        parsed_lines: int,
        failed_lines: int,
        status: str = 'completed'
    ) -> None:
        """
        更新分析任务状态

        Args:
            task_id: 任务 ID
            total_lines: 总行数
            parsed_lines: 成功解析行数
            failed_lines: 失败行数
            status: 任务状态
        """
        sql = '''
            UPDATE analysis_tasks
            SET total_lines  = ?,
                parsed_lines = ?,
                failed_lines = ?,
                end_time     = ?,
                status       = ?
            WHERE id = ?
        '''
        with self.conn:
            self.conn.execute(sql, (
                total_lines, parsed_lines, failed_lines,
                datetime.now().isoformat(), status, task_id
            ))
        logger.info(f"任务 #{task_id} 更新完成: 状态={status}")

    # ------------------------------------------------------------------ #
    #  查询方法                                                             #
    # ------------------------------------------------------------------ #

    def get_total_records(self, source_file: Optional[str] = None) -> int:
        """
        获取总记录数

        Args:
            source_file: 筛选特定来源文件（None 表示全部）

        Returns:
            记录总数
        """
        if source_file:
            row = self.conn.execute(
                'SELECT COUNT(*) FROM log_records WHERE source_file = ?',
                (source_file,)
            ).fetchone()
        else:
            row = self.conn.execute(
                'SELECT COUNT(*) FROM log_records'
            ).fetchone()
        return row[0]

    def get_unique_ips(self, source_file: Optional[str] = None) -> int:
        """获取独立 IP 数（UV）"""
        if source_file:
            row = self.conn.execute(
                'SELECT COUNT(DISTINCT ip) FROM log_records WHERE source_file = ?',
                (source_file,)
            ).fetchone()
        else:
            row = self.conn.execute(
                'SELECT COUNT(DISTINCT ip) FROM log_records'
            ).fetchone()
        return row[0]

    def get_status_code_stats(self, source_file: Optional[str] = None) -> list[dict]:
        """
        获取 HTTP 状态码分布

        Returns:
            [{'status_code': 200, 'count': 1000}, ...]
        """
        if source_file:
            rows = self.conn.execute('''
                SELECT status_code, COUNT(*) as count
                FROM log_records
                WHERE source_file = ?
                GROUP BY status_code
                ORDER BY count DESC
            ''', (source_file,)).fetchall()
        else:
            rows = self.conn.execute('''
                SELECT status_code, COUNT(*) as count
                FROM log_records
                GROUP BY status_code
                ORDER BY count DESC
            ''').fetchall()
        return [dict(row) for row in rows]

    def get_top_ips(self, top_n: int = 10, source_file: Optional[str] = None) -> list[dict]:
        """
        获取访问量最多的 IP

        Returns:
            [{'ip': '1.2.3.4', 'count': 500}, ...]
        """
        if source_file:
            rows = self.conn.execute('''
                SELECT ip, COUNT(*) as count
                FROM log_records
                WHERE source_file = ?
                GROUP BY ip
                ORDER BY count DESC
                LIMIT ?
            ''', (source_file, top_n)).fetchall()
        else:
            rows = self.conn.execute('''
                SELECT ip, COUNT(*) as count
                FROM log_records
                GROUP BY ip
                ORDER BY count DESC
                LIMIT ?
            ''', (top_n,)).fetchall()
        return [dict(row) for row in rows]

    def get_top_urls(self, top_n: int = 10, source_file: Optional[str] = None) -> list[dict]:
        """
        获取访问量最多的 URL

        Returns:
            [{'url': '/index.html', 'count': 2000}, ...]
        """
        if source_file:
            rows = self.conn.execute('''
                SELECT url, COUNT(*) as count
                FROM log_records
                WHERE source_file = ?
                GROUP BY url
                ORDER BY count DESC
                LIMIT ?
            ''', (source_file, top_n)).fetchall()
        else:
            rows = self.conn.execute('''
                SELECT url, COUNT(*) as count
                FROM log_records
                GROUP BY url
                ORDER BY count DESC
                LIMIT ?
            ''', (top_n,)).fetchall()
        return [dict(row) for row in rows]

    def get_response_time_stats(self, source_file: Optional[str] = None) -> dict:
        """
        获取响应时间统计（平均值、最大值、最小值）

        Returns:
            {'avg': 0.15, 'max': 2.3, 'min': 0.01, 'total': 1500.0}
        """
        if source_file:
            row = self.conn.execute('''
                SELECT
                    AVG(response_time) as avg,
                    MAX(response_time) as max,
                    MIN(response_time) as min,
                    SUM(response_time) as total
                FROM log_records
                WHERE source_file = ? AND response_time IS NOT NULL
            ''', (source_file,)).fetchone()
        else:
            row = self.conn.execute('''
                SELECT
                    AVG(response_time) as avg,
                    MAX(response_time) as max,
                    MIN(response_time) as min,
                    SUM(response_time) as total
                FROM log_records
                WHERE response_time IS NOT NULL
            ''').fetchone()
        return dict(row) if row else {}

    def get_hourly_stats(self, source_file: Optional[str] = None) -> list[dict]:
        """
        按小时统计访问量（利用 SQLite 字符串截取）

        Returns:
            [{'hour': '2024-01-01 12', 'count': 300}, ...]
        """
        # timestamp 格式：2024-01-01 12:00:00
        if source_file:
            rows = self.conn.execute('''
                SELECT SUBSTR(timestamp, 1, 13) as hour, COUNT(*) as count
                FROM log_records
                WHERE source_file = ?
                GROUP BY hour
                ORDER BY hour
            ''', (source_file,)).fetchall()
        else:
            rows = self.conn.execute('''
                SELECT SUBSTR(timestamp, 1, 13) as hour, COUNT(*) as count
                FROM log_records
                GROUP BY hour
                ORDER BY hour
            ''').fetchall()
        return [dict(row) for row in rows]

    def get_bytes_stats(self, source_file: Optional[str] = None) -> dict:
        """获取流量统计（总字节数）"""
        if source_file:
            row = self.conn.execute('''
                SELECT SUM(bytes_sent) as total_bytes, AVG(bytes_sent) as avg_bytes
                FROM log_records
                WHERE source_file = ? AND bytes_sent IS NOT NULL
            ''', (source_file,)).fetchone()
        else:
            row = self.conn.execute('''
                SELECT SUM(bytes_sent) as total_bytes, AVG(bytes_sent) as avg_bytes
                FROM log_records
                WHERE bytes_sent IS NOT NULL
            ''').fetchone()
        return dict(row) if row else {}

    def get_task_history(self, limit: int = 20) -> list[dict]:
        """
        获取历史分析任务列表

        Returns:
            任务记录列表（最新优先）
        """
        rows = self.conn.execute('''
            SELECT * FROM analysis_tasks
            ORDER BY id DESC
            LIMIT ?
        ''', (limit,)).fetchall()
        return [dict(row) for row in rows]

    def clear_all(self) -> None:
        """清空所有数据（谨慎使用）"""
        with self.conn:
            self.conn.execute('DELETE FROM log_records')
            self.conn.execute('DELETE FROM analysis_tasks')
        logger.warning("数据库已清空")

    def close(self) -> None:
        """关闭当前线程的数据库连接"""
        if hasattr(_thread_local, 'connection'):
            _thread_local.connection.close()
            del _thread_local.connection
            logger.debug(f"线程 {threading.current_thread().name} 关闭数据库连接")


# ------------------------------------------------------------------ #
#  示例用法                                                            #
# ------------------------------------------------------------------ #
if __name__ == '__main__':
    import tempfile
    import os

    logging.basicConfig(level=logging.DEBUG)

    # 使用临时文件测试
    with tempfile.NamedTemporaryFile(suffix='.db', delete=False) as f:
        tmp_db = f.name

    try:
        db = Database(tmp_db)

        # 创建任务
        task_id = db.create_task('access.log', 'apache')

        # 批量插入测试数据
        records = [
            {
                'source_file': 'access.log',
                'ip': f'192.168.1.{i % 20}',
                'timestamp': f'2024-01-01 {i % 24:02d}:00:00',
                'method': 'GET',
                'url': f'/page{i % 5}.html',
                'protocol': 'HTTP/1.1',
                'status_code': 200 if i % 10 != 0 else 404,
                'bytes_sent': 1024 * (i % 10 + 1),
                'response_time': round(0.1 * (i % 10 + 1), 3),
                'user_agent': 'Mozilla/5.0',
                'referer': None,
            }
            for i in range(100)
        ]
        db.insert_log_batch(records)

        # 更新任务状态
        db.update_task(task_id, 100, 100, 0)

        # 查询统计
        print(f"总记录数: {db.get_total_records()}")
        print(f"独立 IP 数: {db.get_unique_ips()}")
        print(f"状态码分布: {db.get_status_code_stats()}")
        print(f"响应时间: {db.get_response_time_stats()}")
        print(f"TOP 3 IP: {db.get_top_ips(3)}")
        print(f"TOP 3 URL: {db.get_top_urls(3)}")
        print(f"历史任务: {db.get_task_history()}")

        db.close()
    finally:
        os.unlink(tmp_db)
