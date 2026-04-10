"""自定义日志 Handler，解决文件锁定导致日志轮转失败的问题（主要影响 Windows）"""
import os
import logging.handlers


class SafeTimedRotatingFileHandler(logging.handlers.TimedRotatingFileHandler):
    """
    安全的按时间轮转日志 Handler，兼容所有平台。

    在 Windows 上，当多个线程/进程同时写入同一个日志文件时，
    TimedRotatingFileHandler 的 doRollover() 会因文件被锁定而抛出 PermissionError。
    此 Handler 捕获该异常并跳过本次轮转，继续写入当前文件，下次达到轮转条件时重试。
    Linux 不存在此问题（POSIX 允许 rename 已打开的文件），捕获逻辑不会影响正常行为。
    """

    def doRollover(self):
        try:
            super().doRollover()
        except PermissionError:
            # Windows 文件锁定，跳过本次轮转，下次重试
            pass

    def rotate(self, source, dest):
        """重写 rotate 方法，处理文件锁定（主要影响 Windows）"""
        if not os.path.exists(source):
            return
        try:
            os.rename(source, dest)
        except PermissionError:
            # 目标文件可能也被锁定，尝试先删除
            try:
                os.remove(dest)
                os.rename(source, dest)
            except PermissionError:
                # 仍然失败则跳过，下次重试
                pass
