"""文件变更监控中间件 — 检测 idip_commands.json 变更并自动同步"""
import os
import logging
import threading
import time

from django.conf import settings
from django.core.cache import cache

logger = logging.getLogger(__name__)

# 缓存键
CACHE_KEY_MTIME = 'gmtool:idip_json_mtime'
CACHE_KEY_LAST_CHECK = 'gmtool:idip_json_last_check'
# 检查间隔（秒），避免每次请求都读取文件
CHECK_INTERVAL = 30


class IDIPFileMonitorMiddleware:
    """
    中间件：监控 idip_commands.json 文件变更。
    
    工作机制：
    1. 每隔 CHECK_INTERVAL 秒检查一次文件的最后修改时间
    2. 如果文件被修改，自动调用 sync_commands_to_db 同步命令
    3. 同步时新增的命令会自动授予超级管理员（由 command_parser 保证）
    """

    def __init__(self, get_response):
        self.get_response = get_response
        # 线程锁，防止并发同步
        self._lock = threading.Lock()

    def __call__(self, request):
        self._check_file_change()
        response = self.get_response(request)
        return response

    def _check_file_change(self):
        """检查 JSON 文件是否被修改，如果修改则自动同步"""
        json_path = os.path.join(settings.BASE_DIR, 'idip_commands.json')
        if not os.path.exists(json_path):
            return

        # 节流：使用缓存控制检查频率
        last_check = cache.get(CACHE_KEY_LAST_CHECK, 0)
        now = time.time()
        if now - last_check < CHECK_INTERVAL:
            return

        # 更新最后检查时间
        cache.set(CACHE_KEY_LAST_CHECK, now, timeout=CHECK_INTERVAL * 2)

        # 获取文件修改时间
        try:
            current_mtime = os.path.getmtime(json_path)
        except OSError:
            return

        # 对比缓存的修改时间
        cached_mtime = cache.get(CACHE_KEY_MTIME)
        if cached_mtime is None:
            # 首次启动，缓存当前 mtime，不做同步
            cache.set(CACHE_KEY_MTIME, current_mtime, timeout=86400)
            return

        if current_mtime > cached_mtime:
            # 文件已变更，需要同步
            with self._lock:
                # 双重检查，防止锁等待期间已被其他线程同步
                cached_mtime2 = cache.get(CACHE_KEY_MTIME)
                if cached_mtime2 is not None and current_mtime <= cached_mtime2:
                    return

                try:
                    from .command_parser import sync_commands_to_db
                    created, updated, deactivated = sync_commands_to_db()
                    if created > 0 or deactivated > 0:
                        logger.info(
                            'idip_commands.json 变更已自动同步: 新增=%d, 更新=%d, 停用=%d',
                            created, updated, deactivated,
                        )
                    # 更新缓存的 mtime
                    cache.set(CACHE_KEY_MTIME, current_mtime, timeout=86400)
                except Exception as e:
                    logger.error('自动同步 idip_commands.json 失败: %s', e)
