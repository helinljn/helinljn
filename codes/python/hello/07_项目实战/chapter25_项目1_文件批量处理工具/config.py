"""
配置管理模块
负责读取和管理配置文件，以及提供默认配置

知识点：
- configparser 模块的使用
- 类型注解
- 面向对象设计
- 单例模式
"""

import configparser
import logging
from pathlib import Path
from typing import Optional

# ============================================================
# 默认配置
# ============================================================

# 默认配置字典（作为 fallback 使用）
DEFAULT_CONFIG: dict = {
    "log_level":       "INFO",
    "log_file":        "file_processor.log",
    "log_max_bytes":   "1048576",   # 1MB
    "log_backup_count": "3",
    "backup_enabled":  "true",
    "backup_suffix":   ".bak",
    "recursive":       "true",
    "ignore_hidden":   "true",
    "dry_run":         "false",
    "encoding":        "utf-8",
}


# ============================================================
# 配置管理类
# ============================================================

class Config:
    """
    配置管理类（单例模式）

    支持从 config.ini 文件读取配置，未设置的项使用默认值。
    命令行参数可以在运行时覆盖配置文件中的设置。

    使用示例：
        config = Config("config.ini")
        log_level = config.get("log_level")
        recursive = config.get_bool("recursive")
    """

    _instance: Optional["Config"] = None

    def __new__(cls, config_file: Optional[str] = None) -> "Config":
        """单例模式：确保全局只有一个配置实例"""
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._initialized = False
        return cls._instance

    def __init__(self, config_file: Optional[str] = None) -> None:
        """
        初始化配置管理器

        Args:
            config_file: 配置文件路径，若为 None 则仅使用默认配置
        """
        # 单例模式下避免重复初始化
        if self._initialized:
            return

        self._config_file = config_file
        self._parser = configparser.ConfigParser(defaults=DEFAULT_CONFIG)

        # 尝试读取配置文件
        if config_file and Path(config_file).exists():
            self._parser.read(config_file, encoding="utf-8")
            print(f"[Config] 已加载配置文件：{config_file}")
        else:
            if config_file:
                print(f"[Config] 配置文件不存在，使用默认配置：{config_file}")

        # 运行时覆盖配置（命令行参数优先级最高）
        self._overrides: dict = {}

        self._initialized = True

    @classmethod
    def reset(cls) -> None:
        """重置单例（用于测试）"""
        cls._instance = None

    # ----------------------------------------------------------
    # 获取配置值
    # ----------------------------------------------------------

    def get(self, key: str, section: str = "DEFAULT") -> str:
        """
        获取字符串配置值

        Args:
            key:     配置键名
            section: 配置段名（默认 DEFAULT）

        Returns:
            配置值字符串
        """
        # 命令行覆盖优先
        if key in self._overrides:
            return self._overrides[key]

        try:
            return self._parser.get(section, key)
        except (configparser.NoSectionError, configparser.NoOptionError):
            # 找不到时返回全局默认值
            return DEFAULT_CONFIG.get(key, "")

    def get_bool(self, key: str, section: str = "DEFAULT") -> bool:
        """
        获取布尔配置值

        Args:
            key:     配置键名
            section: 配置段名

        Returns:
            True 或 False
        """
        if key in self._overrides:
            val = self._overrides[key].lower()
            return val in ("true", "1", "yes", "on")

        try:
            return self._parser.getboolean(section, key)
        except (configparser.NoSectionError, configparser.NoOptionError):
            default = DEFAULT_CONFIG.get(key, "false")
            return default.lower() in ("true", "1", "yes", "on")

    def get_int(self, key: str, section: str = "DEFAULT") -> int:
        """
        获取整数配置值

        Args:
            key:     配置键名
            section: 配置段名

        Returns:
            整数值
        """
        if key in self._overrides:
            return int(self._overrides[key])

        try:
            return self._parser.getint(section, key)
        except (configparser.NoSectionError, configparser.NoOptionError):
            return int(DEFAULT_CONFIG.get(key, "0"))

    def set_override(self, key: str, value: str) -> None:
        """
        设置运行时覆盖配置（命令行参数调用此方法）

        Args:
            key:   配置键名
            value: 配置值（字符串形式）
        """
        self._overrides[key] = str(value)

    def apply_args(self, args: object) -> None:
        """
        将 argparse 解析的参数应用到配置中

        Args:
            args: argparse.Namespace 对象
        """
        args_dict = vars(args)

        # 映射：命令行参数名 -> 配置键名
        mapping = {
            "dry_run":   "dry_run",
            "recursive": "recursive",
            "encoding":  "encoding",
            "log_level": "log_level",
            "log_file":  "log_file",
            "backup":    "backup_enabled",
        }

        for arg_name, config_key in mapping.items():
            if arg_name in args_dict and args_dict[arg_name] is not None:
                val = args_dict[arg_name]
                self.set_override(config_key, str(val))

    def dump(self) -> None:
        """打印当前所有有效配置（调试用）"""
        print("\n========== 当前配置 ==========")
        print(f"  配置文件: {self._config_file or '(无)'}")
        print("  [DEFAULT]")
        for key in DEFAULT_CONFIG:
            print(f"    {key:25s} = {self.get(key)}")
        print("  [覆盖项]")
        for key, val in self._overrides.items():
            print(f"    {key:25s} = {val}")
        print("================================\n")


# ============================================================
# 日志初始化函数
# ============================================================

def setup_logging(config: Config) -> logging.Logger:
    """
    根据配置初始化日志系统

    Args:
        config: Config 实例

    Returns:
        配置好的 Logger
    """
    from logging.handlers import RotatingFileHandler

    log_level_str = config.get("log_level").upper()
    log_level     = getattr(logging, log_level_str, logging.INFO)
    log_file      = config.get("log_file")
    max_bytes     = config.get_int("log_max_bytes")
    backup_count  = config.get_int("log_backup_count")

    # 日志格式
    fmt = logging.Formatter(
        fmt="%(asctime)s [%(levelname)s] %(name)s - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
    )

    logger = logging.getLogger("file_processor")
    logger.setLevel(log_level)

    # 避免重复添加 handler（reload 时常见问题）
    if logger.handlers:
        logger.handlers.clear()

    # 控制台 Handler
    console_handler = logging.StreamHandler()
    console_handler.setLevel(log_level)
    console_handler.setFormatter(fmt)
    logger.addHandler(console_handler)

    # 文件 Handler（带轮转）
    if log_file:
        try:
            file_handler = RotatingFileHandler(
                filename=log_file,
                maxBytes=max_bytes,
                backupCount=backup_count,
                encoding="utf-8",
            )
            file_handler.setLevel(log_level)
            file_handler.setFormatter(fmt)
            logger.addHandler(file_handler)
        except OSError as e:
            logger.warning(f"无法创建日志文件 {log_file}: {e}")

    return logger


# ============================================================
# 生成默认配置文件
# ============================================================

def generate_default_config(output_path: str = "config.ini") -> None:
    """
    生成默认配置文件（方便用户参考和修改）

    Args:
        output_path: 输出文件路径
    """
    content = """\
; ============================================================
; 文件批量处理工具 - 配置文件
; ============================================================

[DEFAULT]
; 日志级别: DEBUG / INFO / WARNING / ERROR / CRITICAL
log_level = INFO

; 日志文件路径（相对路径或绝对路径）
log_file = file_processor.log

; 日志文件最大大小（字节），默认 1MB
log_max_bytes = 1048576

; 日志文件保留份数
log_backup_count = 3

; 是否启用备份（重命名/替换前备份原文件）
backup_enabled = true

; 备份文件后缀
backup_suffix = .bak

; 是否递归搜索子目录
recursive = true

; 是否忽略隐藏文件（以 . 开头）
ignore_hidden = true

; 是否为演练模式（不实际修改文件，仅显示将要执行的操作）
dry_run = false

; 文本文件默认编码
encoding = utf-8


[SEARCH]
; 搜索相关配置（继承 DEFAULT，可单独覆盖）
recursive = true
ignore_hidden = true


[RENAME]
; 重命名相关配置
dry_run = false
create_backup = true


[REPLACE]
; 内容替换相关配置
dry_run = false
backup_enabled = true
encoding = utf-8
"""
    path = Path(output_path)
    path.write_text(content, encoding="utf-8")
    print(f"[Config] 默认配置文件已生成：{output_path}")


# ============================================================
# 模块自测
# ============================================================

if __name__ == "__main__":
    # 1. 生成默认配置文件
    generate_default_config("config.ini")

    # 2. 加载配置
    Config.reset()
    cfg = Config("config.ini")

    # 3. 读取各类型配置值
    print("log_level     :", cfg.get("log_level"))
    print("recursive     :", cfg.get_bool("recursive"))
    print("log_max_bytes :", cfg.get_int("log_max_bytes"))
    print("dry_run       :", cfg.get_bool("dry_run"))

    # 4. 运行时覆盖
    cfg.set_override("dry_run", "true")
    cfg.set_override("log_level", "DEBUG")
    print("\n--- 覆盖后 ---")
    print("dry_run   :", cfg.get_bool("dry_run"))
    print("log_level :", cfg.get("log_level"))

    # 5. 打印全部配置
    cfg.dump()

    # 6. 初始化日志
    logger = setup_logging(cfg)
    logger.info("配置管理模块自测完成")
    logger.debug("调试信息示例")
