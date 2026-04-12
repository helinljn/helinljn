from django.apps import AppConfig
from django.utils.translation import gettext_lazy as _


class GmtoolConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'gmtool'
    verbose_name = _('GM命令管理')

    def ready(self):
        """应用启动时注册信号处理器"""
        import gmtool.signals  # noqa: F401
