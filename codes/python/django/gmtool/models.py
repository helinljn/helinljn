from django.db import models
from django.contrib.auth.models import User
from django.utils.translation import gettext_lazy as _


class GMCommand(models.Model):
    """GM命令定义，从idip_commands.json同步"""
    command_id = models.CharField(_('Command ID'), max_length=20, unique=True)
    command_name = models.CharField(_('Command Name'), max_length=200)
    tab = models.CharField(_('Tab Group'), max_length=200)
    request_name = models.CharField(_('Request Name'), max_length=100)
    request_id = models.IntegerField(_('Request ID'))
    response_name = models.CharField(_('Response Name'), max_length=100)
    response_id = models.IntegerField(_('Response ID'))
    request_params = models.JSONField(_('Request Params'), default=list)
    response_params = models.JSONField(_('Response Params'), default=list)
    is_active = models.BooleanField(_('Active'), default=True)

    class Meta:
        verbose_name = _('GM Command')
        verbose_name_plural = _('GM Commands')
        ordering = ['command_id']

    def __str__(self):
        return f'{self.command_id} - {self.command_name}'


class Role(models.Model):
    """角色定义"""
    name = models.CharField(_('Role Name'), max_length=50, unique=True)
    display_name = models.CharField(_('Display Name'), max_length=100)
    description = models.TextField(_('Description'), blank=True)
    is_super_admin = models.BooleanField(_('Super Admin'), default=False)
    created_at = models.DateTimeField(_('Created At'), auto_now_add=True)

    class Meta:
        verbose_name = _('Role')
        verbose_name_plural = _('Roles')
        ordering = ['-is_super_admin', 'name']

    def __str__(self):
        return self.display_name


class UserCommandPermission(models.Model):
    """用户-命令权限关联（直接按用户分配权限）"""
    user = models.ForeignKey(User, on_delete=models.CASCADE, verbose_name=_('User'))
    command = models.ForeignKey(GMCommand, on_delete=models.CASCADE, verbose_name=_('Command'))

    class Meta:
        verbose_name = _('User Command Permission')
        verbose_name_plural = _('User Command Permissions')
        unique_together = ('user', 'command')

    def __str__(self):
        return f'{self.user.username} - {self.command.command_name}'


class UserProfile(models.Model):
    """用户扩展信息"""
    user = models.OneToOneField(User, on_delete=models.CASCADE, verbose_name=_('User'))
    role = models.ForeignKey(Role, on_delete=models.SET_NULL, null=True, blank=True, verbose_name=_('Role'))
    phone = models.CharField(_('Phone'), max_length=20, blank=True)

    class Meta:
        verbose_name = _('User Profile')
        verbose_name_plural = _('User Profiles')

    def __str__(self):
        return f'{self.user.username} - {self.role.display_name if self.role else _("No Role")}'


class CommandLog(models.Model):
    """命令执行日志"""
    STATUS_CHOICES = [
        ('success', _('Success')),
        ('failed', _('Failed')),
        ('timeout', _('Timeout')),
    ]
    user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True, verbose_name=_('User'))
    command = models.ForeignKey(GMCommand, on_delete=models.SET_NULL, null=True, verbose_name=_('Command'))
    partition = models.IntegerField(_('Partition'))
    request_data = models.JSONField(_('Request Data'))
    request_content = models.TextField(_('Full Request JSON'), blank=True, default='', help_text=_('Full IDIP request content'))
    response_data = models.JSONField(_('Response Data'), null=True, blank=True)
    status = models.CharField(_('Status'), max_length=20, choices=STATUS_CHOICES)
    ip_address = models.GenericIPAddressField(_('IP Address'))
    created_at = models.DateTimeField(_('Created At'), auto_now_add=True)

    class Meta:
        verbose_name = _('Command Log')
        verbose_name_plural = _('Command Logs')
        ordering = ['-created_at']

    def __str__(self):
        return f'{self.user} - {self.command} - {self.status}'


class LoginLog(models.Model):
    """登录日志"""
    ACTION_CHOICES = [
        ('login', _('Login')),
        ('logout', _('Logout')),
        ('login_failed', _('Login Failed')),
    ]
    user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True, verbose_name=_('User'))
    username = models.CharField(_('Username'), max_length=150)
    action = models.CharField(_('Action'), max_length=20, choices=ACTION_CHOICES)
    ip_address = models.GenericIPAddressField(_('IP Address'))
    user_agent = models.CharField(_('User Agent'), max_length=500, blank=True)
    reason = models.CharField(_('Failure Reason'), max_length=200, blank=True)
    created_at = models.DateTimeField(_('Created At'), auto_now_add=True)

    class Meta:
        verbose_name = _('Login Log')
        verbose_name_plural = _('Login Logs')
        ordering = ['-created_at']

    def __str__(self):
        return f'{self.username} - {self.get_action_display()} - {self.created_at}'
