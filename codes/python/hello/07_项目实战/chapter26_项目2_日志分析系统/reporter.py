"""
报告生成模块
将分析结果输出为不同格式（控制台、JSON、CSV）

知识点：
- json 模块：JSON 序列化
- csv 模块：CSV 文件写入
- 格式化输出：字符串对齐、表格美化
- 抽象基类：定义报告器接口
"""

import json
import csv
import logging
from abc import ABC, abstractmethod
from pathlib import Path
from typing import Any

from analyzer import AnalysisResult

logger = logging.getLogger(__name__)


# ------------------------------------------------------------------ #
#  报告器基类（抽象接口）                                               #
# ------------------------------------------------------------------ #

class Reporter(ABC):
    """
    报告器抽象基类

    定义统一的报告生成接口，子类实现具体格式的输出
    """

    @abstractmethod
    def generate(self, result: AnalysisResult, output: str | None = None) -> None:
        """
        生成报告

        Args:
            result: 分析结果
            output: 输出路径（None 表示输出到控制台）
        """
        pass


# ------------------------------------------------------------------ #
#  控制台报告器                                                         #
# ------------------------------------------------------------------ #

class ConsoleReporter(Reporter):
    """
    控制台报告器

    将分析结果以美化的表格形式输出到控制台
    """

    def generate(self, result: AnalysisResult, output: str | None = None) -> None:
        """生成控制台报告"""
        lines = self._format_report(result)
        report_text = '\n'.join(lines)

        if output:
            Path(output).write_text(report_text, encoding='utf-8')
            logger.info(f"控制台格式报告已保存: {output}")
        else:
            print(report_text)

    def _format_report(self, result: AnalysisResult) -> list[str]:
        """格式化报告内容"""
        lines = []
        basic = result['basic_stats']

        # 标题
        lines.append('=' * 70)
        lines.append('日志分析报告'.center(70))
        lines.append('=' * 70)
        lines.append('')

        # 基本信息
        lines.append(f"生成时间: {result['generated_at']}")
        lines.append(f"分析文件: {', '.join(result['source_files'])}")
        lines.append('')

        # 基本统计
        lines.append('-' * 70)
        lines.append('基本统计'.center(70))
        lines.append('-' * 70)
        lines.append(f"  总访问量 (PV):        {basic['total_requests']:>12,}")
        lines.append(f"  独立访客 (UV):        {basic['unique_ips']:>12,}")
        lines.append(f"  成功率:               {result['success_rate']:>11.2f}%")
        lines.append(f"  错误率:               {result['error_rate']:>11.2f}%")
        lines.append(f"  平均响应时间:         {basic['avg_response_time']:>11.4f}s")
        lines.append(f"  最大响应时间:         {basic['max_response_time']:>11.4f}s")
        lines.append(f"  最小响应时间:         {basic['min_response_time']:>11.4f}s")
        lines.append(f"  总流量:               {self._format_bytes(basic['total_bytes']):>12}")
        lines.append(f"  平均请求大小:         {self._format_bytes(basic['avg_bytes']):>12}")
        lines.append('')

        # 状态码分布
        lines.append('-' * 70)
        lines.append('HTTP 状态码分布'.center(70))
        lines.append('-' * 70)
        lines.append(f"  {'状态码':<10} {'数量':>12} {'占比':>10}")
        lines.append('  ' + '-' * 35)
        for s in result['status_stats'][:10]:
            lines.append(
                f"  {s['status_code']:<10} {s['count']:>12,} {s['percentage']:>9.2f}%"
            )
        lines.append('')

        # TOP IP
        lines.append('-' * 70)
        lines.append(f"TOP {len(result['top_ips'])} 访问 IP".center(70))
        lines.append('-' * 70)
        lines.append(f"  {'排名':<6} {'IP 地址':<18} {'访问次数':>12} {'占比':>10}")
        lines.append('  ' + '-' * 50)
        for item in result['top_ips']:
            lines.append(
                f"  {item['rank']:<6} {item['key']:<18} "
                f"{item['count']:>12,} {item['percentage']:>9.2f}%"
            )
        lines.append('')

        # TOP URL
        lines.append('-' * 70)
        lines.append(f"TOP {len(result['top_urls'])} 访问 URL".center(70))
        lines.append('-' * 70)
        lines.append(f"  {'排名':<6} {'URL':<35} {'访问次数':>12} {'占比':>10}")
        lines.append('  ' + '-' * 65)
        for item in result['top_urls']:
            url = item['key'][:33] + '..' if len(item['key']) > 35 else item['key']
            lines.append(
                f"  {item['rank']:<6} {url:<35} "
                f"{item['count']:>12,} {item['percentage']:>9.2f}%"
            )
        lines.append('')

        # 按小时统计
        hourly = result['hourly_stats']
        if hourly:
            lines.append('-' * 70)
            lines.append('按小时访问统计'.center(70))
            lines.append('-' * 70)
            lines.append(f"  {'时间':<20} {'访问次数':>12}")
            lines.append('  ' + '-' * 35)
            display_count = min(10, len(hourly))
            for item in hourly[:display_count]:
                lines.append(f"  {item['hour']:<20} {item['count']:>12,}")
            if len(hourly) > 20:
                lines.append(f"  {'...':<20} {'...':>12}")
                for item in hourly[-10:]:
                    lines.append(f"  {item['hour']:<20} {item['count']:>12,}")
            lines.append('')

        lines.append('=' * 70)
        return lines

    @staticmethod
    def _format_bytes(bytes_count: int | float) -> str:
        """格式化字节数为人类可读格式"""
        val = int(bytes_count)
        for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
            if val < 1024:
                return f"{val:.2f} {unit}"
            val //= 1024
        return f"{val:.2f} PB"


# ------------------------------------------------------------------ #
#  JSON 报告器                                                          #
# ------------------------------------------------------------------ #

class JSONReporter(Reporter):
    """
    JSON 报告器

    将分析结果序列化为 JSON 格式，便于程序化处理
    """

    def __init__(self, indent: int = 2):
        """
        初始化 JSON 报告器

        Args:
            indent: JSON 缩进空格数
        """
        self.indent = indent

    def generate(self, result: AnalysisResult, output: str | None = None) -> None:
        """生成 JSON 报告"""
        json_str = json.dumps(result, ensure_ascii=False, indent=self.indent)

        if output:
            Path(output).write_text(json_str, encoding='utf-8')
            logger.info(f"JSON 报告已保存: {output}")
        else:
            print(json_str)


# ------------------------------------------------------------------ #
#  CSV 报告器                                                           #
# ------------------------------------------------------------------ #

class CSVReporter(Reporter):
    """
    CSV 报告器

    将分析结果导出为多个 CSV 文件（基本统计、状态码、TOP IP、TOP URL、按小时）
    """

    def generate(self, result: AnalysisResult, output: str | None = None) -> None:
        """
        生成 CSV 报告

        由于分析结果包含多个维度，会以 <name>_basic.csv、<name>_status.csv 等
        形式生成多个文件。

        Args:
            result: 分析结果
            output: 输出文件路径前缀（None 时使用 report.csv）
        """
        if not output:
            output = 'report.csv'

        base_path = Path(output)
        base_name = base_path.stem
        base_dir = base_path.parent
        # 确保输出目录存在
        base_dir.mkdir(parents=True, exist_ok=True)

        files_created = []

        # 1. 基本统计
        basic_file = base_dir / f"{base_name}_basic.csv"
        self._write_basic_stats(result, basic_file)
        files_created.append(str(basic_file))

        # 2. 状态码分布
        status_file = base_dir / f"{base_name}_status.csv"
        self._write_status_stats(result, status_file)
        files_created.append(str(status_file))

        # 3. TOP IP
        ip_file = base_dir / f"{base_name}_top_ips.csv"
        self._write_rank_items(result['top_ips'], ['排名', 'IP地址', '访问次数', '占比(%)'], ip_file)
        files_created.append(str(ip_file))

        # 4. TOP URL
        url_file = base_dir / f"{base_name}_top_urls.csv"
        self._write_rank_items(result['top_urls'], ['排名', 'URL', '访问次数', '占比(%)'], url_file)
        files_created.append(str(url_file))

        # 5. 按小时统计
        hourly_file = base_dir / f"{base_name}_hourly.csv"
        self._write_hourly_stats(result, hourly_file)
        files_created.append(str(hourly_file))

        logger.info(f"CSV 报告已生成: {files_created}")
        print(f"CSV 报告已生成 ({len(files_created)} 个文件):")
        for f in files_created:
            print(f"  {f}")

    def _write_basic_stats(self, result: AnalysisResult, path: Path) -> None:
        """写入基本统计 CSV"""
        basic = result['basic_stats']
        rows = [
            ['指标', '数值'],
            ['生成时间', result['generated_at']],
            ['分析文件', ', '.join(result['source_files'])],
            ['总访问量(PV)', basic['total_requests']],
            ['独立访客(UV)', basic['unique_ips']],
            ['成功率(%)', result['success_rate']],
            ['错误率(%)', result['error_rate']],
            ['平均响应时间(s)', basic['avg_response_time']],
            ['最大响应时间(s)', basic['max_response_time']],
            ['最小响应时间(s)', basic['min_response_time']],
            ['总流量(bytes)', basic['total_bytes']],
            ['平均请求大小(bytes)', basic['avg_bytes']],
        ]
        self._write_csv(path, rows)

    def _write_status_stats(self, result: AnalysisResult, path: Path) -> None:
        """写入状态码分布 CSV"""
        rows = [['状态码', '数量', '占比(%)']]
        for s in result['status_stats']:
            rows.append([s['status_code'], s['count'], s['percentage']])
        self._write_csv(path, rows)

    def _write_rank_items(self, items: list, headers: list, path: Path) -> None:
        """写入排名列表 CSV（IP 或 URL）"""
        rows = [headers]
        for item in items:
            rows.append([item['rank'], item['key'], item['count'], item['percentage']])
        self._write_csv(path, rows)

    def _write_hourly_stats(self, result: AnalysisResult, path: Path) -> None:
        """写入按小时统计 CSV"""
        rows = [['时间段', '访问次数']]
        for item in result['hourly_stats']:
            rows.append([item['hour'], item['count']])
        self._write_csv(path, rows)

    @staticmethod
    def _write_csv(path: Path, rows: list[list]) -> None:
        """通用 CSV 写入方法"""
        with open(path, 'w', newline='', encoding='utf-8-sig') as f:
            # 使用 utf-8-sig（带 BOM），确保 Excel 正常显示中文
            writer = csv.writer(f)
            writer.writerows(rows)


# ------------------------------------------------------------------ #
#  报告器工厂函数                                                       #
# ------------------------------------------------------------------ #

def create_reporter(fmt: str) -> Reporter:
    """
    根据格式名称创建对应的报告器

    Args:
        fmt: 报告格式，支持 'console'/'json'/'csv'

    Returns:
        Reporter 实例

    Raises:
        ValueError: 不支持的格式
    """
    reporters = {
        'console': ConsoleReporter,
        'json': JSONReporter,
        'csv': CSVReporter,
    }
    if fmt not in reporters:
        raise ValueError(
            f"不支持的报告格式: {fmt}，"
            f"可用格式: {list(reporters.keys())}"
        )
    return reporters[fmt]()


# ------------------------------------------------------------------ #
#  示例用法                                                             #
# ------------------------------------------------------------------ #

if __name__ == '__main__':
    import tempfile
    import os
    import random

    logging.basicConfig(level=logging.INFO)

    # 构造模拟分析结果
    mock_result: AnalysisResult = {
        'generated_at': '2024-01-15 12:00:00',
        'source_files': ['access.log'],
        'basic_stats': {
            'total_requests': 10000,
            'unique_ips': 1234,
            'avg_response_time': 0.1562,
            'max_response_time': 2.345,
            'min_response_time': 0.001,
            'total_bytes': 104857600,
            'avg_bytes': 10485.76,
        },
        'status_stats': [
            {'status_code': 200, 'count': 8500, 'percentage': 85.0},
            {'status_code': 404, 'count': 1000, 'percentage': 10.0},
            {'status_code': 500, 'count': 300, 'percentage': 3.0},
            {'status_code': 301, 'count': 200, 'percentage': 2.0},
        ],
        'top_ips': [
            {'rank': i + 1, 'key': f'192.168.1.{i + 1}',
             'count': 500 - i * 30, 'percentage': round((500 - i * 30) / 10000 * 100, 2)}
            for i in range(5)
        ],
        'top_urls': [
            {'rank': 1, 'key': '/index.html', 'count': 3000, 'percentage': 30.0},
            {'rank': 2, 'key': '/api/data', 'count': 2000, 'percentage': 20.0},
            {'rank': 3, 'key': '/about.html', 'count': 1500, 'percentage': 15.0},
            {'rank': 4, 'key': '/images/logo.png', 'count': 1000, 'percentage': 10.0},
            {'rank': 5, 'key': '/api/login', 'count': 800, 'percentage': 8.0},
        ],
        'hourly_stats': [
            {'hour': f'2024-01-15 {h:02d}', 'count': random.randint(100, 800)}
            for h in range(24)
        ],
        'error_rate': 13.0,
        'success_rate': 85.0,
    }

    print("=== 测试报告生成器 ===\n")

    # 1. 控制台报告
    print("--- 控制台报告 ---")
    console_reporter = ConsoleReporter()
    console_reporter.generate(mock_result)

    # 2. JSON 报告（输出到临时文件）
    print("\n--- JSON 报告 ---")
    json_reporter = JSONReporter()
    with tempfile.NamedTemporaryFile(
        suffix='.json', delete=False, mode='w'
    ) as f:
        tmp_json = f.name
    json_reporter.generate(mock_result, tmp_json)
    print(f"JSON 文件大小: {Path(tmp_json).stat().st_size} 字节")
    os.unlink(tmp_json)

    # 3. CSV 报告
    print("\n--- CSV 报告 ---")
    csv_reporter = CSVReporter()
    with tempfile.TemporaryDirectory() as tmp_dir:
        csv_reporter.generate(mock_result, f"{tmp_dir}/report.csv")
        csv_files = list(Path(tmp_dir).glob('*.csv'))
        print(f"共生成 {len(csv_files)} 个 CSV 文件")
        for f in csv_files:
            print(f"  {f.name}: {f.stat().st_size} 字节")

    # 4. 工厂函数测试
    print("\n--- 工厂函数测试 ---")
    for fmt in ['console', 'json', 'csv']:
        r = create_reporter(fmt)
        print(f"  create_reporter('{fmt}') -> {type(r).__name__}")
