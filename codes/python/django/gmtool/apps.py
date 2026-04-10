from django.apps import AppConfig


class GmtoolConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'gmtool'
    verbose_name = 'GM命令管理'

    def ready(self):
        """应用启动时注册信号处理器"""
        import gmtool.signals  # noqa: F401
