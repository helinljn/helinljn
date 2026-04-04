"""
配置管理模块
负责读取和管理配置文件
"""

import configparser
import os
from pathlib import Path
from typing import Any


class Config:
    """配置管理类"""

    def __init__(self, config_file: str = 'config.ini'):
        """
        初始化配置

        Args:
            config_file: 配置文件路径
        """
        self.config_file = config_file
        self.config = configparser.ConfigParser()
        self._load_config()

    def _load_config(self) -> None:
        """加载配置文件"""
        if not os.path.exists(self.config_file):
            raise FileNotFoundError(f"配置文件不存在: {self.config_file}")

        self.config.read(self.config_file, encoding='utf-8')

    def get(self, section: str, option: str, fallback: Any = None) -> str:
        """
        获取配置项

        Args:
            section: 配置节
            option: 配置项
            fallback: 默认值

        Returns:
            配置值
        """
        return self.config.get(section, option, fallback=fallback)

    def getint(self, section: str, option: str, fallback: int = 0) -> int:
        """获取整数配置项"""
        return self.config.getint(section, option, fallback=fallback)

    def getfloat(self, section: str, option: str, fallback: float = 0.0) -> float:
        """获取浮点数配置项"""
        return self.config.getfloat(section, option, fallback=fallback)

    def getboolean(self, section: str, option: str, fallback: bool = False) -> bool:
        """获取布尔配置项"""
        return self.config.getboolean(section, option, fallback=fallback)

    # 数据库配置
    @property
    def db_file(self) -> str:
        """数据库文件路径"""
        return self.get('database', 'db_file', 'logs.db')

    # 解析器配置
    @property
    def default_format(self) -> str:
        """默认日志格式"""
        return self.get('parser', 'default_format', 'apache')

    @property
    def batch_size(self) -> int:
        """批量插入大小"""
        return self.getint('parser', 'batch_size', 1000)

    # 分析器配置
    @property
    def top_n(self) -> int:
        """TOP N 统计数量"""
        return self.getint('analyzer', 'top_n', 10)

    @property
    def time_interval(self) -> int:
        """时间间隔（小时）"""
        return self.getint('analyzer', 'time_interval', 1)

    # 报告器配置
    @property
    def default_report_format(self) -> str:
        """默认报告格式"""
        return self.get('reporter', 'default_format', 'console')

    @property
    def output_dir(self) -> str:
        """输出目录"""
        return self.get('reporter', 'output_dir', 'reports')

    # 日志配置
    @property
    def log_level(self) -> str:
        """日志级别"""
        return self.get('logging', 'level', 'INFO')

    @property
    def log_file(self) -> str:
        """日志文件路径"""
        return self.get('logging', 'log_file', '')

    @property
    def log_max_bytes(self) -> int:
        """日志文件最大大小"""
        return self.getint('logging', 'max_bytes', 10485760)

    @property
    def log_backup_count(self) -> int:
        """日志文件备份数量"""
        return self.getint('logging', 'backup_count', 3)


# 全局配置实例
_config: Config | None = None


def get_config(config_file: str = 'config.ini') -> Config:
    """
    获取全局配置实例（单例模式）

    Args:
        config_file: 配置文件路径

    Returns:
        配置实例
    """
    global _config
    if _config is None:
        _config = Config(config_file)
    return _config


def reload_config(config_file: str = 'config.ini') -> Config:
    """
    重新加载配置

    Args:
        config_file: 配置文件路径

    Returns:
        新的配置实例
    """
    global _config
    _config = Config(config_file)
    return _config


# 示例用法
if __name__ == '__main__':
    # 获取配置
    config = get_config()

    print("=== 配置信息 ===")
    print(f"数据库文件: {config.db_file}")
    print(f"默认日志格式: {config.default_format}")
    print(f"批量大小: {config.batch_size}")
    print(f"TOP N: {config.top_n}")
    print(f"时间间隔: {config.time_interval}小时")
    print(f"默认报告格式: {config.default_report_format}")
    print(f"输出目录: {config.output_dir}")
    print(f"日志级别: {config.log_level}")
    print(f"日志文件: {config.log_file or '(控制台)'}")
