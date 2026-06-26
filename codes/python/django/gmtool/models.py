from django.conf import settings
from django.db import models
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
    field_labels = models.JSONField(
        _('字段标签映射'),
        default=dict,
        help_text=_('已同步字段的 id -> name 映射，用于结果展示（当前不递归展开更深层嵌套结构）'),
    )
    is_active = models.BooleanField(_('是否启用'), default=True)

    class Meta:
        verbose_name = _('GM命令')
        verbose_name_plural = _('GM命令')
        ordering = ['command_id']
        indexes = [
            models.Index(fields=['is_active', 'command_id'], name='gmcmd_active_cmd_idx'),
        ]
        constraints = [
            models.UniqueConstraint(fields=['request_id'], name='uniq_gmcmd_request_id'),
            models.UniqueConstraint(fields=['response_id'], name='uniq_gmcmd_response_id'),
            models.CheckConstraint(
                condition=~models.Q(request_id=models.F('response_id')),
                name='gmcmd_req_resp_distinct',
            ),
        ]

    def __str__(self):
        return f'{self.command_id} - {self.command_name}'


class UserCommandPermission(models.Model):
    """用户-命令权限关联（直接按用户分配权限）"""
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE, verbose_name=_('用户'))
    command = models.ForeignKey(GMCommand, on_delete=models.CASCADE, verbose_name=_('命令'))

    class Meta:
        verbose_name = _('用户命令权限')
        verbose_name_plural = _('用户命令权限')
        constraints = [
            models.UniqueConstraint(
                fields=['user', 'command'],
                name='uniq_user_command_permission',
            )
        ]

    def __str__(self):
        return f'{self.user.username} - {self.command.command_name}'


class UserAnnouncementPermission(models.Model):
    """用户公告管理权限。超级管理员不需要写入此表。"""
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE, verbose_name=_('用户'))

    class Meta:
        verbose_name = _('用户公告权限')
        verbose_name_plural = _('用户公告权限')
        constraints = [
            models.UniqueConstraint(
                fields=['user'],
                name='uniq_user_announcement_permission',
            )
        ]

    def __str__(self):
        return f'{self.user.username} - announcement'


class UserProfile(models.Model):
    """用户扩展信息"""
    user = models.OneToOneField(settings.AUTH_USER_MODEL, on_delete=models.CASCADE, verbose_name=_('用户'))
    phone = models.CharField(_('联系电话'), max_length=20, blank=True)

    class Meta:
        verbose_name = _('用户信息')
        verbose_name_plural = _('用户信息')

    def __str__(self):
        return self.user.username


class CommandLog(models.Model):
    """命令执行日志"""
    STATUS_CHOICES = [
        ('success', _('成功')),
        ('failed', _('失败')),
        ('timeout', _('超时')),
    ]
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.SET_NULL, null=True, verbose_name=_('用户'))
    operator_username = models.CharField(_('操作用户名'), max_length=150, blank=True, default='')
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
        indexes = [
            models.Index(fields=['-created_at'], name='cmdlog_created_idx'),
            models.Index(fields=['user', '-created_at'], name='cmdlog_user_created_idx'),
            models.Index(fields=['status', '-created_at'], name='cmdlog_status_created_idx'),
            models.Index(fields=['command', '-created_at'], name='cmdlog_cmd_created_idx'),
        ]

    def __str__(self):
        return f'{self.user} - {self.command} - {self.status}'


class AnnouncementLog(models.Model):
    """公告发布、删除操作日志。"""
    ACTION_CHOICES = [
        ('create', _('发布')),
        ('delete', _('删除')),
    ]
    STATUS_CHOICES = [
        ('success', _('成功')),
        ('failed', _('失败')),
        ('timeout', _('超时')),
    ]

    user = models.ForeignKey(
        settings.AUTH_USER_MODEL,
        on_delete=models.SET_NULL,
        null=True,
        blank=True,
        verbose_name=_('用户'),
    )
    operator_username = models.CharField(_('操作用户名'), max_length=150, blank=True, default='')
    action = models.CharField(_('操作类型'), max_length=20, choices=ACTION_CHOICES)
    platform = models.CharField(_('平台'), max_length=50)
    channel = models.CharField(_('渠道'), max_length=50)
    announcement_type = models.CharField(_('公告类型'), max_length=10)
    announcement_id = models.CharField(_('公告ID'), max_length=64, blank=True, default='')
    request_data = models.JSONField(_('请求数据'), default=dict)
    response_data = models.JSONField(_('响应数据'), null=True, blank=True)
    raw_response = models.TextField(_('原始响应'), blank=True, default='')
    status = models.CharField(_('状态'), max_length=20, choices=STATUS_CHOICES)
    error_message = models.TextField(_('错误信息'), blank=True, default='')
    ip_address = models.GenericIPAddressField(_('IP地址'))
    created_at = models.DateTimeField(_('创建时间'), auto_now_add=True)

    class Meta:
        verbose_name = _('公告日志')
        verbose_name_plural = _('公告日志')
        ordering = ['-created_at']
        indexes = [
            models.Index(fields=['-created_at'], name='annlog_created_idx'),
            models.Index(fields=['user', '-created_at'], name='annlog_user_created_idx'),
            models.Index(fields=['status', '-created_at'], name='annlog_status_created_idx'),
            models.Index(fields=['action', '-created_at'], name='annlog_action_created_idx'),
            models.Index(fields=['platform', 'channel', '-created_at'], name='annlog_plat_chan_ct_idx'),
        ]

    def __str__(self):
        return f'{self.operator_username} - {self.get_action_display()} - {self.status}'


class AnnouncementReview(models.Model):
    """公告待审核记录。"""
    STATUS_CHOICES = [
        ('pending', _('待审核')),
        ('approved', _('已通过')),
        ('rejected', _('已作废')),
        ('failed', _('发布失败')),
    ]
    ANNOUNCEMENT_TYPE_CHOICES = [
        ('1', _('周更新公告')),
        ('2', _('常驻公告')),
        ('3', _('轮播图')),
    ]

    status = models.CharField(_('审核状态'), max_length=20, choices=STATUS_CHOICES, default='pending')
    submitter = models.ForeignKey(
        settings.AUTH_USER_MODEL,
        on_delete=models.SET_NULL,
        null=True,
        blank=True,
        related_name='submitted_announcement_reviews',
        verbose_name=_('提交人'),
    )
    submitter_username = models.CharField(_('提交用户名'), max_length=150, blank=True, default='')
    reviewer = models.ForeignKey(
        settings.AUTH_USER_MODEL,
        on_delete=models.SET_NULL,
        null=True,
        blank=True,
        related_name='reviewed_announcement_reviews',
        verbose_name=_('审核人'),
    )
    reviewer_username = models.CharField(_('审核用户名'), max_length=150, blank=True, default='')
    platform = models.CharField(_('平台'), max_length=50)
    channel = models.CharField(_('渠道'), max_length=50)
    announcement_type = models.CharField(_('公告类型'), max_length=10, choices=ANNOUNCEMENT_TYPE_CHOICES)
    announcement_id = models.CharField(_('公告ID'), max_length=64, blank=True, default='-1')
    title = models.CharField(_('公告标题'), max_length=200, blank=True, default='')
    content = models.TextField(_('公告正文'), blank=True, default='')
    priority = models.IntegerField(_('显示优先级'), default=0)
    image_1 = models.CharField(_('Image_1'), max_length=500, blank=True, default='')
    image_link_1 = models.CharField(_('ImageLink_1'), max_length=500, blank=True, default='')
    image_2 = models.CharField(_('Image_2'), max_length=500, blank=True, default='')
    image_link_2 = models.CharField(_('ImageLink_2'), max_length=500, blank=True, default='')
    image_3 = models.CharField(_('Image_3'), max_length=500, blank=True, default='')
    image_link_3 = models.CharField(_('ImageLink_3'), max_length=500, blank=True, default='')
    reserve_1 = models.TextField(_('Reserve_1'), blank=True, default='')
    reserve_2 = models.TextField(_('Reserve_2'), blank=True, default='')
    payload = models.JSONField(_('待发布数据'), default=dict)
    review_comment = models.TextField(_('审核备注'), blank=True, default='')
    remote_response = models.JSONField(_('远端响应'), null=True, blank=True)
    raw_response = models.TextField(_('远端原始响应'), blank=True, default='')
    error_message = models.TextField(_('失败原因'), blank=True, default='')
    created_at = models.DateTimeField(_('提交时间'), auto_now_add=True)
    reviewed_at = models.DateTimeField(_('审核时间'), null=True, blank=True)
    updated_at = models.DateTimeField(_('更新时间'), auto_now=True)

    class Meta:
        verbose_name = _('公告审核')
        verbose_name_plural = _('公告审核')
        ordering = ['-created_at']
        indexes = [
            models.Index(fields=['status', '-created_at'], name='annrev_status_created_idx'),
            models.Index(fields=['submitter', '-created_at'], name='annrev_submit_created_idx'),
            models.Index(fields=['reviewer', '-reviewed_at'], name='annrev_reviewed_idx'),
            models.Index(fields=['platform', 'channel', 'status'], name='annrev_plat_chan_stat_idx'),
            models.Index(fields=['announcement_type', 'status'], name='annrev_type_status_idx'),
        ]
        constraints = [
            models.UniqueConstraint(
                fields=['platform', 'channel', 'announcement_type'],
                condition=models.Q(status='pending', announcement_type='1'),
                name='uniq_pending_weekly_review',
            ),
        ]

    def __str__(self):
        return f'{self.platform} - {self.channel} - {self.get_announcement_type_display()} - {self.status}'


class LoginLog(models.Model):
    """登录日志"""
    ACTION_CHOICES = [
        ('login', _('登录')),
        ('logout', _('登出')),
        ('login_failed', _('登录失败')),
    ]
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.SET_NULL, null=True, verbose_name=_('用户'))
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
        indexes = [
            models.Index(fields=['-created_at'], name='loginlog_created_idx'),
            models.Index(fields=['ip_address', '-created_at'], name='loginlog_ip_created_idx'),
            models.Index(fields=['action', '-created_at'], name='loginlog_action_created_idx'),
            models.Index(fields=['username', '-created_at'], name='loginlog_user_created_idx'),
        ]

    def __str__(self):
        return f'{self.username} - {self.get_action_display()} - {self.created_at}'
