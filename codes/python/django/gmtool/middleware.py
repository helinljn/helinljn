"""文件变更监控中间件 — 开发环境下检测 idip_commands.json 变更并自动同步"""
import hashlib
import os
import logging
import threading
import time

from django.conf import settings
from django.core.cache import cache

logger = logging.getLogger(__name__)

# 缓存键
CACHE_KEY_MTIME = 'gmtool:idip_json_mtime'
CACHE_KEY_HASH = 'gmtool:idip_json_hash'
CACHE_KEY_LAST_CHECK = 'gmtool:idip_json_last_check'
# 检查间隔（秒），避免每次请求都读取文件，可通过环境变量配置
CHECK_INTERVAL = getattr(settings, 'IDIP_FILE_CHECK_INTERVAL', 30)
# 是否启用文件监控（默认仅开发环境启用）
ENABLE_FILE_MONITOR = getattr(settings, 'ENABLE_IDIP_FILE_MONITOR', True)
# 是否使用文件哈希进行变更检测（更准确但稍慢）
USE_HASH_CHECK = getattr(settings, 'IDIP_USE_HASH_CHECK', False)


class IDIPFileMonitorMiddleware:
    """
    中间件：监控 idip_commands.json 文件变更。

    工作机制：
    1. 如果启用监控，每隔 CHECK_INTERVAL 秒检查一次文件变更
    2. 支持两种变更检测方式：修改时间（默认）或文件哈希（更准确）
    3. 如果文件被修改，自动调用 sync_commands_to_db 同步命令
    4. 同步时新增的命令会自动授予超级管理员（由 command_parser 保证）

    设计定位：
    - 该能力主要用于开发联调时减少手工同步操作
    - 生产环境建议通过显式执行 `python manage.py sync_commands`
      或上传 JSON 后自动同步，不建议依赖请求中间件作为主同步机制

    注意：当前使用 LocMemCache 和线程锁，仅在单进程部署下有效。
    多进程（如 gunicorn 多 worker）环境下，各进程的缓存不共享，
    可能导致重复同步。如需多进程支持，请配置 Redis 缓存和分布式锁。
    """

    def __init__(self, get_response):
        self.get_response = get_response
        # 线程锁，防止并发同步
        self._lock = threading.Lock()
        # 初始化状态
        self._enabled = ENABLE_FILE_MONITOR
        self._json_path = getattr(settings, 'IDIP_JSON_PATH', os.path.join(settings.BASE_DIR, 'idip_commands.json'))

    def __call__(self, request):
        if self._enabled:
            self._check_file_change()
        return self.get_response(request)

    def _calculate_file_hash(self, filepath):
        """计算文件内容的SHA256哈希值"""
        try:
            with open(filepath, 'rb') as f:
                return hashlib.sha256(f.read()).hexdigest()
        except (OSError, IOError):
            return None

    def _check_file_change(self):
        """检查 JSON 文件是否被修改，如果修改则自动同步"""
        if not os.path.exists(self._json_path):
            return

        # 节流：使用缓存控制检查频率
        last_check = cache.get(CACHE_KEY_LAST_CHECK, 0)
        now = time.time()
        if now - last_check < CHECK_INTERVAL:
            return

        # 更新最后检查时间
        cache.set(CACHE_KEY_LAST_CHECK, now, timeout=CHECK_INTERVAL * 2)

        try:
            current_mtime = os.path.getmtime(self._json_path)
        except OSError:
            return

        # 根据配置选择变更检测方式
        file_changed = False

        if USE_HASH_CHECK:
            # 使用文件哈希检测变更（更准确但性能稍差）
            current_hash = self._calculate_file_hash(self._json_path)
            if current_hash is None:
                return

            cached_hash = cache.get(CACHE_KEY_HASH)
            if cached_hash is None:
                # 首次启动，缓存当前哈希
                cache.set(CACHE_KEY_HASH, current_hash, timeout=86400)
                cache.set(CACHE_KEY_MTIME, current_mtime, timeout=86400)
                return

            if current_hash != cached_hash:
                file_changed = True
                logger.debug('文件哈希变更: 旧=%s, 新=%s', cached_hash[:8], current_hash[:8])
        else:
            # 使用修改时间检测变更（默认）
            cached_mtime = cache.get(CACHE_KEY_MTIME)
            if cached_mtime is None:
                # 首次启动，缓存当前 mtime
                cache.set(CACHE_KEY_MTIME, current_mtime, timeout=86400)
                return

            if current_mtime > cached_mtime:
                file_changed = True
                logger.debug('文件修改时间变更: 旧=%s, 新=%s', cached_mtime, current_mtime)

        if file_changed:
            # 文件已变更，需要同步
            with self._lock:
                # 双重检查，防止锁等待期间已被其他线程同步
                if USE_HASH_CHECK:
                    current_hash2 = self._calculate_file_hash(self._json_path)
                    cached_hash2 = cache.get(CACHE_KEY_HASH)
                    if cached_hash2 is not None and current_hash2 == cached_hash2:
                        return
                else:
                    current_mtime2 = os.path.getmtime(self._json_path)
                    cached_mtime2 = cache.get(CACHE_KEY_MTIME)
                    if cached_mtime2 is not None and current_mtime2 <= cached_mtime2:
                        return

                try:
                    from .command_parser import sync_commands_to_db
                    created, updated, deactivated = sync_commands_to_db()

                    if created > 0 or updated > 0 or deactivated > 0:
                        logger.info(
                            'idip_commands.json 变更已自动同步: 新增=%d, 更新=%d, 停用=%d',
                            created, updated, deactivated,
                        )

                    # 更新缓存的状态
                    if USE_HASH_CHECK:
                        cache.set(CACHE_KEY_HASH, current_hash2, timeout=86400)
                    cache.set(CACHE_KEY_MTIME, current_mtime2 if not USE_HASH_CHECK else current_mtime, timeout=86400)

                except Exception as e:
                    logger.error('自动同步 idip_commands.json 失败: %s', e)
                    # 即使同步失败，也更新缓存时间，避免重复尝试
                    if not USE_HASH_CHECK:
                        cache.set(CACHE_KEY_MTIME, current_mtime, timeout=86400)
