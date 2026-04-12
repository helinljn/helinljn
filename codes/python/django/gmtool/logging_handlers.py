"""自定义日志 Handler，让活跃日志文件从启动起就带日期，并安全处理轮转"""
import glob
import os
import re
import time
import logging.handlers


class SafeTimedRotatingFileHandler(logging.handlers.TimedRotatingFileHandler):
    """
    安全的按时间轮转日志 Handler，兼容所有平台。

    与标准 TimedRotatingFileHandler 的区别：
    1. 当前活跃日志文件从启动起就带日期：audit-2026-04-12.log（而非 audit.log）
    2. 轮转时直接切换到新日期文件，无需重命名（完全避免 Windows 文件锁问题）
    3. 轮转文件命名格式：audit-2026-04-12.log（日期嵌入文件名中间）

    settings.py 中配置的 filename（如 audit.log）仅作为基础名称，
    用于计算带日期的实际文件名，该文件本身不会被创建。
    """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # 设置自定义 namer，将 "audit.log.2026-04-12" 改为 "audit-2026-04-12.log"
        self.namer = self._custom_namer

    @staticmethod
    def _custom_namer(default_name):
        """
        将默认轮转文件名从 'xxx.log.2026-04-12' 转换为 'xxx-2026-04-12.log'

        例如：
            /path/to/audit.log.2026-04-12  ->  /path/to/audit-2026-04-12.log
            /path/to/gmtool.log.2026-04-12 ->  /path/to/gmtool-2026-04-12.log
        """
        # default_name 格式: /path/to/audit.log.2026-04-12
        base, date_suffix = default_name.rsplit('.', 1)   # base='.../audit.log', date_suffix='2026-04-12'
        name_without_ext, ext = os.path.splitext(base)     # name_without_ext='.../audit', ext='.log'
        return f"{name_without_ext}-{date_suffix}{ext}"    # '.../audit-2026-04-12.log'

    def _open(self):
        """
        重写 _open，使活跃日志文件从启动起就带日期。

        标准库行为：打开 audit.log，轮转时才重命名为 audit.log.2026-04-12
        本 Handler：直接打开 audit-2026-04-12.log，轮转时切换到 audit-2026-04-13.log

        这样做的好处：
        - 日志文件从第一条记录起就带日期，格式统一
        - 轮转时无需重命名文件，完全避免 Windows 文件锁问题

        注意：_open() 可能在 super().__init__() 内部被调用，此时 self.utc
        和 self.suffix 尚未设置，因此使用 getattr 提供安全的默认值。
        """
        utc = getattr(self, 'utc', False)
        suffix = getattr(self, 'suffix', '%Y-%m-%d')
        if utc:
            t = time.gmtime()
        else:
            t = time.localtime()
        date_str = time.strftime(suffix, t)
        dated_name = self._custom_namer(self.baseFilename + "." + date_str)
        os.makedirs(os.path.dirname(dated_name), exist_ok=True)
        return open(dated_name, self.mode, encoding=self.encoding)

    def getFilesToDelete(self):
        """
        重写旧日志清理逻辑，匹配新的命名格式 'xxx-2026-04-12.log'。

        标准库的 getFilesToDelete 基于 suffix 模式匹配后缀，
        但我们的格式是日期嵌入文件名中间，需要自定义匹配逻辑。
        """
        dirName, baseName = os.path.split(self.baseFilename)
        fileNameWithoutExt, ext = os.path.splitext(baseName)

        # 匹配格式：audit-YYYY-MM-DD.log
        pattern = os.path.join(dirName, f"{fileNameWithoutExt}-*{ext}")
        fileNames = glob.glob(pattern)

        # 用正则精确过滤，避免误匹配
        # 例如匹配 audit-2026-04-12.log 但不匹配 audit-backup.log
        date_pattern = re.compile(
            re.escape(fileNameWithoutExt) + r'-\d{4}-\d{2}-\d{2}' + re.escape(ext) + r'$'
        )
        result = [
            fn for fn in fileNames
            if date_pattern.search(os.path.basename(fn))
        ]

        if len(result) < self.backupCount:
            result = []
        else:
            result.sort()
            result = result[:len(result) - self.backupCount]
        return result

    def doRollover(self):
        """
        重写 doRollover，直接切换到新日期文件，无需重命名。

        与标准库的区别：
        - 不执行任何文件重命名操作（完全避免 Windows 文件锁问题）
        - 关闭旧流后，_open() 自动根据当前日期打开新文件
        - 始终更新 rolloverAt，避免重复触发轮转
        """
        currentTime = int(time.time())

        # 关闭当前流
        if self.stream:
            self.stream.close()
            self.stream = None

        # 清理旧备份（忽略失败）
        if self.backupCount > 0:
            try:
                for s in self.getFilesToDelete():
                    try:
                        os.remove(s)
                    except OSError:
                        pass
            except Exception:
                pass

        # 重新打开文件（_open 会自动使用新日期）
        if not self.delay:
            self.stream = self._open()

        # 始终更新轮转时间，避免每次 emit 都重试
        self.rolloverAt = self.computeRollover(currentTime)
