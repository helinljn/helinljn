from django.db import models
from django.contrib.auth.models import User


class GMCommand(models.Model):
    """GM命令定义，从idip_commands.json同步"""
    command_id = models.CharField('命令编号', max_length=20, unique=True)
    command_name = models.CharField('命令名称', max_length=200)
    tab = models.CharField('分组标签', max_length=200)
    request_name = models.CharField('请求名', max_length=100)
    request_id = models.IntegerField('协议请求ID')
    response_name = models.CharField('响应名', max_length=100)
    response_id = models.IntegerField('协议响应ID')
    request_params = models.JSONField('请求参数定义', default=list)
    response_params = models.JSONField('响应参数定义', default=list)
    is_active = models.BooleanField('是否启用', default=True)

    class Meta:
        verbose_name = 'GM命令'
        verbose_name_plural = 'GM命令'
        ordering = ['command_id']

    def __str__(self):
        return f'{self.command_id} - {self.command_name}'


class Role(models.Model):
    """角色定义"""
    name = models.CharField('角色标识', max_length=50, unique=True)
    display_name = models.CharField('显示名称', max_length=100)
    description = models.TextField('角色描述', blank=True)
    is_super_admin = models.BooleanField('超级管理员', default=False)
    created_at = models.DateTimeField('创建时间', auto_now_add=True)

    class Meta:
        verbose_name = '角色'
        verbose_name_plural = '角色'
        ordering = ['-is_super_admin', 'name']

    def __str__(self):
        return self.display_name


class UserCommandPermission(models.Model):
    """用户-命令权限关联（直接按用户分配权限）"""
    user = models.ForeignKey(User, on_delete=models.CASCADE, verbose_name='用户')
    command = models.ForeignKey(GMCommand, on_delete=models.CASCADE, verbose_name='命令')

    class Meta:
        verbose_name = '用户命令权限'
        verbose_name_plural = '用户命令权限'
        unique_together = ('user', 'command')

    def __str__(self):
        return f'{self.user.username} - {self.command.command_name}'


class UserProfile(models.Model):
    """用户扩展信息"""
    user = models.OneToOneField(User, on_delete=models.CASCADE, verbose_name='用户')
    role = models.ForeignKey(Role, on_delete=models.SET_NULL, null=True, blank=True, verbose_name='角色分组')
    phone = models.CharField('联系电话', max_length=20, blank=True)

    class Meta:
        verbose_name = '用户信息'
        verbose_name_plural = '用户信息'

    def __str__(self):
        return f'{self.user.username} - {self.role.display_name if self.role else "无分组"}'


class CommandLog(models.Model):
    """命令执行日志"""
    STATUS_CHOICES = [
        ('success', '成功'),
        ('failed', '失败'),
        ('timeout', '超时'),
    ]
    user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True, verbose_name='操作用户')
    command = models.ForeignKey(GMCommand, on_delete=models.SET_NULL, null=True, verbose_name='执行命令')
    partition = models.IntegerField('服务器组号')
    request_data = models.JSONField('请求数据')
    response_data = models.JSONField('响应数据', null=True, blank=True)
    status = models.CharField('状态', max_length=20, choices=STATUS_CHOICES)
    ip_address = models.GenericIPAddressField('IP地址')
    created_at = models.DateTimeField('执行时间', auto_now_add=True)

    class Meta:
        verbose_name = '命令日志'
        verbose_name_plural = '命令日志'
        ordering = ['-created_at']

    def __str__(self):
        return f'{self.user} - {self.command} - {self.status}'


class LoginLog(models.Model):
    """登录日志"""
    ACTION_CHOICES = [
        ('login', '登录'),
        ('logout', '登出'),
        ('login_failed', '登录失败'),
    ]
    user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True, verbose_name='用户')
    username = models.CharField('登录用户名', max_length=150)
    action = models.CharField('操作类型', max_length=20, choices=ACTION_CHOICES)
    ip_address = models.GenericIPAddressField('IP地址')
    user_agent = models.CharField('浏览器标识', max_length=500, blank=True)
    reason = models.CharField('失败原因', max_length=200, blank=True)
    created_at = models.DateTimeField('操作时间', auto_now_add=True)

    class Meta:
        verbose_name = '登录日志'
        verbose_name_plural = '登录日志'
        ordering = ['-created_at']

    def __str__(self):
        return f'{self.username} - {self.get_action_display()} - {self.created_at}'
