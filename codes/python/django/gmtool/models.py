from django.db import models
from django.contrib.auth.models import User
from django.utils.translation import gettext_lazy as _


class GMCommand(models.Model):
    """GM命令定义，从idip_commands.json同步"""
    command_id = models.CharField(_('命令编号'), max_length=20, unique=True)
    command_name = models.CharField(_('命令名称'), max_length=200)
    tab = models.CharField(_('分组标签'), max_length=200)
    request_name = models.CharField(_('请求名'), max_length=100)
    request_id = models.IntegerField(_('协议请求ID'))
    response_name = models.CharField(_('响应名'), max_length=100)
    response_id = models.IntegerField(_('协议响应ID'))
    request_params = models.JSONField(_('请求参数定义'), default=list)
    response_params = models.JSONField(_('响应参数定义'), default=list)
    is_active = models.BooleanField(_('是否启用'), default=True)

    class Meta:
        verbose_name = _('GM命令')
        verbose_name_plural = _('GM命令')
        ordering = ['command_id']

    def __str__(self):
        return f'{self.command_id} - {self.command_name}'


class Role(models.Model):
    """角色定义"""
    name = models.CharField(_('角色标识'), max_length=50, unique=True)
    display_name = models.CharField(_('显示名称'), max_length=100)
    description = models.TextField(_('角色描述'), blank=True)
    is_super_admin = models.BooleanField(_('超级管理员'), default=False)
    created_at = models.DateTimeField(_('创建时间'), auto_now_add=True)

    class Meta:
        verbose_name = _('角色')
        verbose_name_plural = _('角色')
        ordering = ['-is_super_admin', 'name']

    def __str__(self):
        return self.display_name


class UserCommandPermission(models.Model):
    """用户-命令权限关联（直接按用户分配权限）"""
    user = models.ForeignKey(User, on_delete=models.CASCADE, verbose_name=_('用户'))
    command = models.ForeignKey(GMCommand, on_delete=models.CASCADE, verbose_name=_('命令'))

    class Meta:
        verbose_name = _('用户命令权限')
        verbose_name_plural = _('用户命令权限')
        unique_together = ('user', 'command')

    def __str__(self):
        return f'{self.user.username} - {self.command.command_name}'


class UserProfile(models.Model):
    """用户扩展信息"""
    user = models.OneToOneField(User, on_delete=models.CASCADE, verbose_name=_('用户'))
    role = models.ForeignKey(Role, on_delete=models.SET_NULL, null=True, blank=True, verbose_name=_('角色'))
    phone = models.CharField(_('联系电话'), max_length=20, blank=True)

    class Meta:
        verbose_name = _('用户信息')
        verbose_name_plural = _('用户信息')

    def __str__(self):
        return f'{self.user.username} - {self.role.display_name if self.role else _("无分组")}'


class CommandLog(models.Model):
    """命令执行日志"""
    STATUS_CHOICES = [
        ('success', _('成功')),
        ('failed', _('失败')),
        ('timeout', _('超时')),
    ]
    user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True, verbose_name=_('用户'))
    command = models.ForeignKey(GMCommand, on_delete=models.SET_NULL, null=True, verbose_name=_('命令'))
    partition = models.IntegerField(_('服务器组号'))
    request_data = models.JSONField(_('请求数据'))
    request_content = models.TextField(_('完整请求JSON'), blank=True, default='', help_text=_('IDIP协议完整请求内容'))
    response_data = models.JSONField(_('响应数据'), null=True, blank=True)
    status = models.CharField(_('状态'), max_length=20, choices=STATUS_CHOICES)
    ip_address = models.GenericIPAddressField(_('IP地址'))
    created_at = models.DateTimeField(_('创建时间'), auto_now_add=True)

    class Meta:
        verbose_name = _('命令日志')
        verbose_name_plural = _('命令日志')
        ordering = ['-created_at']

    def __str__(self):
        return f'{self.user} - {self.command} - {self.status}'


class LoginLog(models.Model):
    """登录日志"""
    ACTION_CHOICES = [
        ('login', _('登录')),
        ('logout', _('登出')),
        ('login_failed', _('登录失败')),
    ]
    user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True, verbose_name=_('用户'))
    username = models.CharField(_('用户名'), max_length=150)
    action = models.CharField(_('操作类型'), max_length=20, choices=ACTION_CHOICES)
    ip_address = models.GenericIPAddressField(_('IP地址'))
    user_agent = models.CharField(_('浏览器标识'), max_length=500, blank=True)
    reason = models.CharField(_('失败原因'), max_length=200, blank=True)
    created_at = models.DateTimeField(_('创建时间'), auto_now_add=True)

    class Meta:
        verbose_name = _('登录日志')
        verbose_name_plural = _('登录日志')
        ordering = ['-created_at']

    def __str__(self):
        return f'{self.username} - {self.get_action_display()} - {self.created_at}'
