"""
日志分析系统 - 主程序入口

综合运用知识点：
- argparse：命令行参数解析
- logging：日志记录配置
- 多模块协作：解析器、数据库、分析器、报告器
- 异常处理：完善的错误处理
- 多线程：并发解析多个文件
"""

import sys
import logging
import argparse
import threading
from pathlib import Path
from logging.handlers import RotatingFileHandler

from config import get_config
from database import Database
from log_parser import LogParser
from analyzer import LogAnalyzer
from reporter import create_reporter


def setup_logging(config) -> None:
    """配置日志系统（控制台 + 可选文件，支持滚动）"""
    log_level = getattr(logging, config.log_level.upper(), logging.INFO)
    fmt = logging.Formatter(
        '%(asctime)s [%(levelname)s] %(name)s: %(message)s',
        '%Y-%m-%d %H:%M:%S'
    )
    handlers: list[logging.Handler] = [logging.StreamHandler(sys.stdout)]
    handlers[0].setFormatter(fmt)

    if config.log_file:
        fh = RotatingFileHandler(
            config.log_file,
            maxBytes=config.log_max_bytes,
            backupCount=config.log_backup_count,
            encoding='utf-8'
        )
        fh.setFormatter(fmt)
        handlers.append(fh)

    logging.basicConfig(level=log_level, handlers=handlers, force=True)


def parse_arguments() -> argparse.Namespace:
    """解析命令行参数"""
    parser = argparse.ArgumentParser(
        description='日志分析系统 - 解析、分析和统计日志文件',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python main.py -f sample_logs/access.log
  python main.py -f sample_logs/access.log sample_logs/nginx.log --threads 2
  python main.py -f sample_logs/access.log --log-format apache -o report.json --format json
  python main.py -f sample_logs/access.log --format csv -o report.csv
  python main.py --history
  python main.py --clear-db
        """
    )

    parser.add_argument('-f', '--files', nargs='+', metavar='FILE',
                        help='要分析的日志文件（可指定多个）')
    parser.add_argument('--log-format', choices=['apache', 'nginx', 'combined'],
                        help='日志格式（默认读取配置文件）')
    parser.add_argument('-o', '--output', metavar='FILE',
                        help='报告输出路径（不指定则输出到控制台）')
    parser.add_argument('--format', choices=['console', 'json', 'csv'],
                        help='报告格式（默认读取配置文件）')
    parser.add_argument('--threads', type=int, metavar='N',
                        help='并发线程数（默认 1）')
    parser.add_argument('--batch-size', type=int, metavar='N',
                        help='批量插入记录数（默认读取配置文件）')
    parser.add_argument('--top-n', type=int, metavar='N',
                        help='TOP N 统计数量（默认读取配置文件）')
    parser.add_argument('--history', action='store_true',
                        help='查看历史分析任务')
    parser.add_argument('--clear-db', action='store_true',
                        help='清空数据库（谨慎使用）')
    parser.add_argument('--config', default='config.ini', metavar='FILE',
                        help='配置文件路径（默认: config.ini）')
    return parser.parse_args()


def analyze_logs(args, config, db: Database) -> None:
    """执行日志分析主流程"""
    logger = logging.getLogger(__name__)

    if not args.files:
        logger.error("未指定日志文件，请使用 -f 参数")
        sys.exit(1)

    # 检查文件
    for fp in args.files:
        if not Path(fp).exists():
            logger.error(f"文件不存在: {fp}")
            sys.exit(1)

    log_format = args.log_format or config.default_format
    batch_size = args.batch_size or config.batch_size
    threads = args.threads or 1
    top_n = args.top_n or config.top_n
    report_format = args.format or config.default_report_format

    logger.info(
        f"分析 {len(args.files)} 个文件 | "
        f"格式={log_format} | 批量={batch_size} | 线程={threads}"
    )

    lp = LogParser(log_format=log_format, batch_size=batch_size)
    db_lock = threading.Lock()

    def on_batch(batch: list[dict]) -> None:
        with db_lock:
            db.insert_log_batch(batch)

    # ---- 解析 ----
    if threads > 1 and len(args.files) > 1:
        logger.info(f"多线程并发解析（{threads} 线程）")
        stats = lp.parse_files_concurrent(args.files, on_batch, max_workers=threads)
    else:
        stats = {'total': 0, 'parsed': 0, 'failed': 0}
        for fp in args.files:
            tid = db.create_task(fp, log_format)
            try:
                t, p, f = lp.parse_file_in_batches(fp, on_batch)
                db.update_task(tid, t, p, f, 'completed')
                stats['total'] += t
                stats['parsed'] += p
                stats['failed'] += f
            except Exception as e:
                logger.error(f"解析失败 {fp}: {e}")
                db.update_task(tid, 0, 0, 0, 'failed')
                raise

    logger.info(
        f"解析完成: 总={stats['total']}, "
        f"成功={stats['parsed']}, 失败={stats['failed']}"
    )

    if stats['parsed'] == 0:
        logger.warning("无有效记录，跳过报告生成")
        return

    # ---- 分析 ----
    logger.info("开始数据分析...")
    analyzer = LogAnalyzer(db, top_n=top_n)
    result = analyzer.analyze(source_files=args.files)
    logger.info(f"分析结果: {analyzer.summarize(result)}")

    # ---- 报告 ----
    logger.info(f"生成 {report_format} 格式报告...")
    reporter = create_reporter(report_format)
    reporter.generate(result, args.output)
    if args.output:
        logger.info(f"报告已保存至: {args.output}")


def show_history(db: Database) -> None:
    """显示历史分析任务列表"""
    tasks = db.get_task_history(limit=20)
    if not tasks:
        print("暂无历史分析记录")
        return

    sep = "=" * 105
    print(f"\n{sep}")
    print("历史分析任务".center(105))
    print(sep)
    print(
        f"{'ID':<6} {'文件':<30} {'格式':<10} "
        f"{'总行数':>10} {'成功':>10} {'失败':>10} "
        f"{'状态':<12} {'开始时间':<20}"
    )
    print("-" * 105)
    for t in tasks:
        fname = t['source_file']
        if len(fname) > 28:
            fname = fname[:25] + '...'
        status_map = {'running': '运行中', 'completed': '已完成', 'failed': '失败'}
        status = status_map.get(t['status'], t['status'])
        print(
            f"{t['id']:<6} {fname:<30} {t['log_format']:<10} "
            f"{t['total_lines']:>10} {t['parsed_lines']:>10} {t['failed_lines']:>10} "
            f"{status:<12} {t['start_time'][:19]:<20}"
        )
    print(sep)


def confirm_clear(prompt: str) -> bool:
    """二次确认危险操作"""
    answer = input(f"{prompt} [输入 yes 确认]: ").strip().lower()
    return answer == 'yes'


def main() -> None:
    """程序主入口"""
    args = parse_arguments()

    # 加载配置（切换工作目录到脚本所在位置，保证配置文件路径正确）
    import os
    script_dir = Path(__file__).parent
    os.chdir(script_dir)

    try:
        config = get_config(args.config)
    except FileNotFoundError as e:
        print(f"错误: {e}", file=sys.stderr)
        sys.exit(1)

    # 初始化日志
    setup_logging(config)
    logger = logging.getLogger(__name__)
    logger.info("日志分析系统启动")

    # 初始化数据库
    db = Database(config.db_file)

    try:
        # ---- 清空数据库 ----
        if args.clear_db:
            if confirm_clear("警告：即将清空所有历史数据！"):
                db.clear_all()
                print("数据库已清空")
            else:
                print("操作已取消")
            return

        # ---- 查看历史 ----
        if args.history:
            show_history(db)
            return

        # ---- 分析日志 ----
        if args.files:
            analyze_logs(args, config, db)
        else:
            # 无参数时打印帮助
            print("未指定操作，请使用 --help 查看用法")
            print("快速示例: python main.py -f sample_logs/access.log")

    except KeyboardInterrupt:
        logger.info("用户中断操作")
    except Exception as e:
        logger.exception(f"程序异常退出: {e}")
        sys.exit(1)
    finally:
        db.close()
        logger.info("日志分析系统退出")


if __name__ == '__main__':
    main()
