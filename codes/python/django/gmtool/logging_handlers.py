"""自定义日志 Handler，解决文件锁定导致日志轮转失败的问题（主要影响 Windows）"""
import os
import time
import logging.handlers


class SafeTimedRotatingFileHandler(logging.handlers.TimedRotatingFileHandler):
    """
    安全的按时间轮转日志 Handler，兼容所有平台。

    在 Windows 上，当多个线程/进程同时写入同一个日志文件时，
    TimedRotatingFileHandler 的 doRollover() 会因文件被锁定而抛出 PermissionError。
    此 Handler 完全重写 doRollover()，确保轮转失败时文件流仍能正常恢复，
    日志不会丢失。Linux 不存在此问题（POSIX 允许 rename 已打开的文件）。
    """

    def doRollover(self):
        """
        重写 doRollover，保证即使文件重命名失败，日志流也能恢复。

        与标准库的区别：
        - rotate 失败时不抛异常，仅跳过文件重命名
        - 始终确保 self.stream 被重新打开
        - 始终更新 rolloverAt，避免重复触发轮转
        """
        currentTime = int(time.time())
        t = self.rolloverAt - self.interval
        if self.utc:
            timeTuple = time.gmtime(t)
        else:
            timeTuple = time.localtime(t)
            dstNow = time.localtime(currentTime)[-1]
            dstThen = timeTuple[-1]
            if dstNow != dstThen:
                if dstNow:
                    addend = 3600
                else:
                    addend = -3600
                timeTuple = time.localtime(t + addend)

        dfn = self.rotation_filename(
            self.baseFilename + "." + time.strftime(self.suffix, timeTuple)
        )

        if os.path.exists(dfn):
            # 已存在同名轮转文件，说明轮转已完成（可能由其他进程完成）
            # 更新 rolloverAt 即可
            self.rolloverAt = self.computeRollover(currentTime)
            return

        # 关闭当前流
        if self.stream:
            self.stream.close()
            self.stream = None

        # 尝试重命名文件（Windows 上可能因文件锁定而失败）
        self.rotate(self.baseFilename, dfn)

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

        # 始终重新打开文件流，确保日志不中断
        if not self.delay:
            self.stream = self._open()

        # 始终更新轮转时间，避免每次 emit 都重试
        self.rolloverAt = self.computeRollover(currentTime)

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
            except (PermissionError, FileNotFoundError):
                # Windows 文件锁定导致重命名失败，跳过本次轮转
                pass
        except FileNotFoundError:
            # 极端情况：source 在检查后被删除
            pass
