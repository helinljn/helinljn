"""
数据分析模块
从数据库中查询并聚合统计数据，生成分析结果

知识点：
- collections.Counter：快速计数统计
- 统计学基础：平均值、百分比计算
- 数据聚合：多维度统计
- 类型注解：完整的 TypedDict 使用
"""

import logging
from collections import Counter
from datetime import datetime
from typing import Optional, TypedDict

from database import Database

logger = logging.getLogger(__name__)


# ------------------------------------------------------------------ #
#  类型定义（TypedDict 让结构更清晰）                                   #
# ------------------------------------------------------------------ #

class BasicStats(TypedDict):
    """基本访问统计"""
    total_requests: int       # 总请求数（PV）
    unique_ips: int           # 独立 IP 数（UV）
    avg_response_time: float  # 平均响应时间
    max_response_time: float  # 最大响应时间
    min_response_time: float  # 最小响应时间
    total_bytes: int          # 总流量（字节）
    avg_bytes: float          # 平均每次请求字节数


class StatusStats(TypedDict):
    """状态码统计"""
    status_code: int
    count: int
    percentage: float


class RankItem(TypedDict):
    """排名项"""
    rank: int
    key: str
    count: int
    percentage: float


class HourlyItem(TypedDict):
    """按小时统计项"""
    hour: str
    count: int


class AnalysisResult(TypedDict):
    """完整分析结果"""
    generated_at: str              # 报告生成时间
    source_files: list[str]        # 分析的文件列表
    basic_stats: BasicStats        # 基本统计
    status_stats: list[StatusStats]  # 状态码分布
    top_ips: list[RankItem]        # 访问量最多的 IP
    top_urls: list[RankItem]       # 访问量最多的 URL
    hourly_stats: list[HourlyItem]  # 按小时统计
    error_rate: float              # 错误率（4xx + 5xx）
    success_rate: float            # 成功率（2xx）


# ------------------------------------------------------------------ #
#  分析器类                                                             #
# ------------------------------------------------------------------ #

class LogAnalyzer:
    """
    日志数据分析器

    从 Database 中读取数据，计算各维度统计指标，
    返回结构化的 AnalysisResult 供报告器使用。
    """

    def __init__(self, db: Database, top_n: int = 10):
        """
        初始化分析器

        Args:
            db: 数据库实例
            top_n: TOP N 统计的 N 值
        """
        self.db = db
        self.top_n = top_n

    def _calc_percentage(self, part: int, total: int) -> float:
        """计算百分比（避免除零）"""
        return round(part / total * 100, 2) if total > 0 else 0.0

    def analyze(self, source_files: Optional[list[str]] = None) -> AnalysisResult:
        """
        执行完整分析，返回所有统计指标

        Args:
            source_files: 要分析的文件列表（None 表示分析全部）

        Returns:
            完整的 AnalysisResult 字典
        """
        # 当有多个文件时，分别查询后合并；单文件或全局直接查询
        # 为简洁，这里传 source_file=None 查全库，或传第一个文件
        # 实际生产可以扩展为多文件合并
        source_file = source_files[0] if source_files and len(source_files) == 1 else None

        logger.info(f"开始分析数据 (source_file={source_file or '全库'})")

        # 1. 基本统计
        basic = self._analyze_basic(source_file)

        # 2. 状态码分布
        status_list = self._analyze_status_codes(source_file, basic['total_requests'])

        # 3. TOP IP
        top_ips = self._analyze_top_ips(source_file, basic['total_requests'])

        # 4. TOP URL
        top_urls = self._analyze_top_urls(source_file, basic['total_requests'])

        # 5. 按小时统计
        hourly = self._analyze_hourly(source_file)

        # 6. 计算错误率和成功率
        error_count = sum(
            s['count'] for s in status_list
            if s['status_code'] and s['status_code'] >= 400
        )
        success_count = sum(
            s['count'] for s in status_list
            if s['status_code'] and 200 <= s['status_code'] < 300
        )
        total = basic['total_requests']

        result: AnalysisResult = {
            'generated_at': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
            'source_files': source_files or ['(全库)'],
            'basic_stats': basic,
            'status_stats': status_list,
            'top_ips': top_ips,
            'top_urls': top_urls,
            'hourly_stats': hourly,
            'error_rate': self._calc_percentage(error_count, total),
            'success_rate': self._calc_percentage(success_count, total),
        }

        logger.info("分析完成")
        return result

    def _analyze_basic(self, source_file: Optional[str]) -> BasicStats:
        """
        计算基本统计指标

        Returns:
            BasicStats 字典
        """
        total = self.db.get_total_records(source_file)
        unique_ips = self.db.get_unique_ips(source_file)
        resp_stats = self.db.get_response_time_stats(source_file)
        bytes_stats = self.db.get_bytes_stats(source_file)

        return BasicStats(
            total_requests=total,
            unique_ips=unique_ips,
            avg_response_time=round(resp_stats.get('avg') or 0.0, 4),
            max_response_time=round(resp_stats.get('max') or 0.0, 4),
            min_response_time=round(resp_stats.get('min') or 0.0, 4),
            total_bytes=int(bytes_stats.get('total_bytes') or 0),
            avg_bytes=round(bytes_stats.get('avg_bytes') or 0.0, 2),
        )

    def _analyze_status_codes(
        self,
        source_file: Optional[str],
        total: int
    ) -> list[StatusStats]:
        """
        计算各状态码的分布和百分比

        Returns:
            按出现次数降序排列的状态码统计列表
        """
        raw = self.db.get_status_code_stats(source_file)
        result = []
        for item in raw:
            result.append(StatusStats(
                status_code=item['status_code'],
                count=item['count'],
                percentage=self._calc_percentage(item['count'], total),
            ))
        return result

    def _analyze_top_ips(
        self,
        source_file: Optional[str],
        total: int
    ) -> list[RankItem]:
        """
        计算访问量 TOP N 的 IP 列表

        Returns:
            带排名和百分比的 IP 列表
        """
        raw = self.db.get_top_ips(self.top_n, source_file)
        result = []
        for rank, item in enumerate(raw, start=1):
            result.append(RankItem(
                rank=rank,
                key=item['ip'],
                count=item['count'],
                percentage=self._calc_percentage(item['count'], total),
            ))
        return result

    def _analyze_top_urls(
        self,
        source_file: Optional[str],
        total: int
    ) -> list[RankItem]:
        """
        计算访问量 TOP N 的 URL 列表

        Returns:
            带排名和百分比的 URL 列表
        """
        raw = self.db.get_top_urls(self.top_n, source_file)
        result = []
        for rank, item in enumerate(raw, start=1):
            result.append(RankItem(
                rank=rank,
                key=item['url'],
                count=item['count'],
                percentage=self._calc_percentage(item['count'], total),
            ))
        return result

    def _analyze_hourly(self, source_file: Optional[str]) -> list[HourlyItem]:
        """
        按小时统计访问量

        Returns:
            按时间升序排列的小时统计列表
        """
        raw = self.db.get_hourly_stats(source_file)
        return [
            HourlyItem(hour=item['hour'], count=item['count'])
            for item in raw
        ]

    # ---------------------------------------------------------------- #
    #  便捷分析方法                                                       #
    # ---------------------------------------------------------------- #

    def get_peak_hour(self, source_file: Optional[str] = None) -> Optional[HourlyItem]:
        """
        获取访问量峰值时段

        Returns:
            访问量最高的小时统计项，无数据返回 None
        """
        hourly = self._analyze_hourly(source_file)
        if not hourly:
            return None
        return max(hourly, key=lambda x: x['count'])

    def get_slow_requests_summary(
        self,
        threshold: float = 1.0,
        source_file: Optional[str] = None
    ) -> dict:
        """
        统计慢请求（响应时间超过阈值）的数量

        Args:
            threshold: 慢请求阈值（秒）
            source_file: 来源文件筛选

        Returns:
            {'threshold': 1.0, 'slow_count': 50, 'total': 10000, 'percentage': 0.5}
        """
        # 从数据库直接查询慢请求计数
        if source_file:
            row = self.db.conn.execute(
                'SELECT COUNT(*) FROM log_records '
                'WHERE response_time > ? AND source_file = ?',
                (threshold, source_file)
            ).fetchone()
        else:
            row = self.db.conn.execute(
                'SELECT COUNT(*) FROM log_records WHERE response_time > ?',
                (threshold,)
            ).fetchone()

        slow_count = row[0] if row else 0
        total = self.db.get_total_records(source_file)

        return {
            'threshold': threshold,
            'slow_count': slow_count,
            'total': total,
            'percentage': self._calc_percentage(slow_count, total),
        }

    def get_error_urls(
        self,
        top_n: Optional[int] = None,
        source_file: Optional[str] = None
    ) -> list[dict]:
        """
        统计出现错误最多的 URL（状态码 >= 400）

        Args:
            top_n: 返回前 N 条（None 则用默认值）
            source_file: 来源文件筛选

        Returns:
            [{'url': '/not-found', 'count': 500, 'status_codes': [404]}, ...]
        """
        n = top_n or self.top_n
        if source_file:
            rows = self.db.conn.execute('''
                SELECT url, COUNT(*) as count,
                       GROUP_CONCAT(DISTINCT status_code) as codes
                FROM log_records
                WHERE status_code >= 400 AND source_file = ?
                GROUP BY url
                ORDER BY count DESC
                LIMIT ?
            ''', (source_file, n)).fetchall()
        else:
            rows = self.db.conn.execute('''
                SELECT url, COUNT(*) as count,
                       GROUP_CONCAT(DISTINCT status_code) as codes
                FROM log_records
                WHERE status_code >= 400
                GROUP BY url
                ORDER BY count DESC
                LIMIT ?
            ''', (n,)).fetchall()

        return [
            {
                'url': row['url'],
                'count': row['count'],
                'status_codes': [int(c) for c in (row['codes'] or '').split(',') if c],
            }
            for row in rows
        ]

    def summarize(self, result: AnalysisResult) -> str:
        """
        将分析结果格式化为简短摘要字符串（供日志输出）

        Args:
            result: 分析结果

        Returns:
            多行摘要字符串
        """
        basic = result['basic_stats']
        lines = [
            f"总请求: {basic['total_requests']:,}",
            f"独立IP: {basic['unique_ips']:,}",
            f"成功率: {result['success_rate']}%",
            f"错误率: {result['error_rate']}%",
            f"平均响应: {basic['avg_response_time']}s",
        ]
        return ' | '.join(lines)


# ------------------------------------------------------------------ #
#  示例用法                                                             #
# ------------------------------------------------------------------ #

if __name__ == '__main__':
    import tempfile
    import os

    logging.basicConfig(level=logging.INFO)

    with tempfile.NamedTemporaryFile(suffix='.db', delete=False) as f:
        tmp_db = f.name

    try:
        db = Database(tmp_db)

        # 插入模拟数据
        import random
        random.seed(42)
        urls = ['/index.html', '/about.html', '/api/data', '/api/login', '/images/logo.png']
        ips = [f'192.168.1.{i}' for i in range(1, 21)]
        statuses = [200] * 80 + [404] * 10 + [500] * 5 + [301] * 5

        records = []
        for i in range(500):
            hour = random.randint(0, 23)
            records.append({
                'source_file': 'test.log',
                'ip': random.choice(ips),
                'timestamp': f'2024-01-15 {hour:02d}:{random.randint(0,59):02d}:00',
                'method': random.choice(['GET', 'POST']),
                'url': random.choice(urls),
                'protocol': 'HTTP/1.1',
                'status_code': random.choice(statuses),
                'bytes_sent': random.randint(256, 10240),
                'response_time': round(random.uniform(0.01, 2.0), 3),
                'user_agent': 'Mozilla/5.0',
                'referer': None,
            })

        db.insert_log_batch(records)

        # 执行分析
        analyzer = LogAnalyzer(db, top_n=5)
        result = analyzer.analyze(source_files=['test.log'])

        print("=== 分析结果摘要 ===")
        print(analyzer.summarize(result))

        print("\n--- 基本统计 ---")
        for k, v in result['basic_stats'].items():
            print(f"  {k}: {v}")

        print("\n--- 状态码分布 ---")
        for s in result['status_stats']:
            print(f"  {s['status_code']}: {s['count']} ({s['percentage']}%)")

        print("\n--- TOP 5 URL ---")
        for item in result['top_urls']:
            print(f"  {item['rank']}. {item['key']}: {item['count']} ({item['percentage']}%)")

        print("\n--- 峰值时段 ---")
        peak = analyzer.get_peak_hour('test.log')
        print(f"  {peak}")

        print("\n--- 慢请求统计（>1s）---")
        slow = analyzer.get_slow_requests_summary(1.0, 'test.log')
        print(f"  {slow}")

        print("\n--- 错误 URL TOP 5 ---")
        for item in analyzer.get_error_urls(5, 'test.log'):
            print(f"  {item}")

        db.close()
    finally:
        os.unlink(tmp_db)
