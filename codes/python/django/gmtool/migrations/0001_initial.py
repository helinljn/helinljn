import django.db.models.deletion
from django.conf import settings
from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='GMCommand',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('command_id', models.CharField(max_length=20, unique=True, verbose_name='命令编号')),
                ('command_name', models.CharField(max_length=200, verbose_name='命令名称')),
                ('tab', models.CharField(max_length=200, verbose_name='分组标签')),
                ('request_name', models.CharField(max_length=100, verbose_name='请求名')),
                ('request_id', models.IntegerField(verbose_name='协议请求ID')),
                ('response_name', models.CharField(max_length=100, verbose_name='响应名')),
                ('response_id', models.IntegerField(verbose_name='协议响应ID')),
                ('request_params', models.JSONField(default=list, verbose_name='请求参数定义')),
                ('response_params', models.JSONField(default=list, verbose_name='响应参数定义')),
                ('field_labels', models.JSONField(default=dict, help_text='所有字段（含嵌套结构体）的 id -> name 映射，用于结果展示', verbose_name='字段标签映射')),
                ('is_active', models.BooleanField(default=True, verbose_name='是否启用')),
            ],
            options={
                'verbose_name': 'GM命令',
                'verbose_name_plural': 'GM命令',
                'ordering': ['command_id'],
            },
        ),
        migrations.CreateModel(
            name='Role',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=50, unique=True, verbose_name='角色标识')),
                ('display_name', models.CharField(max_length=100, verbose_name='显示名称')),
                ('description', models.TextField(blank=True, verbose_name='角色描述')),
                ('is_super_admin', models.BooleanField(default=False, verbose_name='超级管理员')),
                ('created_at', models.DateTimeField(auto_now_add=True, verbose_name='创建时间')),
            ],
            options={
                'verbose_name': '角色',
                'verbose_name_plural': '角色',
                'ordering': ['-is_super_admin', 'name'],
            },
        ),
        migrations.CreateModel(
            name='UserCommandPermission',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('command', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='gmtool.gmcommand', verbose_name='命令')),
                ('user', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL, verbose_name='用户')),
            ],
            options={
                'verbose_name': '用户命令权限',
                'verbose_name_plural': '用户命令权限',
            },
        ),
        migrations.CreateModel(
            name='UserProfile',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('phone', models.CharField(blank=True, max_length=20, verbose_name='联系电话')),
                ('role', models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.SET_NULL, to='gmtool.role', verbose_name='角色')),
                ('user', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL, verbose_name='用户')),
            ],
            options={
                'verbose_name': '用户信息',
                'verbose_name_plural': '用户信息',
            },
        ),
        migrations.CreateModel(
            name='CommandLog',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('operator_username', models.CharField(blank=True, default='', max_length=150, verbose_name='操作用户名')),
                ('partition', models.IntegerField(verbose_name='服务器组号')),
                ('request_data', models.JSONField(verbose_name='请求数据')),
                ('request_content', models.TextField(blank=True, default='', help_text='IDIP协议完整请求内容', verbose_name='完整请求JSON')),
                ('response_data', models.JSONField(blank=True, null=True, verbose_name='响应数据')),
                ('status', models.CharField(choices=[('success', '成功'), ('failed', '失败'), ('timeout', '超时')], max_length=20, verbose_name='状态')),
                ('ip_address', models.GenericIPAddressField(verbose_name='IP地址')),
                ('created_at', models.DateTimeField(auto_now_add=True, verbose_name='创建时间')),
                ('command', models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, to='gmtool.gmcommand', verbose_name='命令')),
                ('user', models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, to=settings.AUTH_USER_MODEL, verbose_name='用户')),
            ],
            options={
                'verbose_name': '命令日志',
                'verbose_name_plural': '命令日志',
                'ordering': ['-created_at'],
            },
        ),
        migrations.CreateModel(
            name='LoginLog',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('username', models.CharField(max_length=150, verbose_name='用户名')),
                ('action', models.CharField(choices=[('login', '登录'), ('logout', '登出'), ('login_failed', '登录失败')], max_length=20, verbose_name='操作类型')),
                ('ip_address', models.GenericIPAddressField(verbose_name='IP地址')),
                ('user_agent', models.CharField(blank=True, max_length=500, verbose_name='浏览器标识')),
                ('reason', models.CharField(blank=True, max_length=200, verbose_name='失败原因')),
                ('created_at', models.DateTimeField(auto_now_add=True, verbose_name='创建时间')),
                ('user', models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, to=settings.AUTH_USER_MODEL, verbose_name='用户')),
            ],
            options={
                'verbose_name': '登录日志',
                'verbose_name_plural': '登录日志',
                'ordering': ['-created_at'],
            },
        ),
        migrations.AddConstraint(
            model_name='usercommandpermission',
            constraint=models.UniqueConstraint(fields=('user', 'command'), name='uniq_user_command_permission'),
        ),
        migrations.AddIndex(
            model_name='gmcommand',
            index=models.Index(fields=['is_active', 'command_id'], name='gmcmd_active_cmd_idx'),
        ),
        migrations.AddIndex(
            model_name='gmcommand',
            index=models.Index(fields=['request_id'], name='gmcmd_reqid_idx'),
        ),
        migrations.AddIndex(
            model_name='gmcommand',
            index=models.Index(fields=['response_id'], name='gmcmd_respid_idx'),
        ),
        migrations.AddIndex(
            model_name='commandlog',
            index=models.Index(fields=['-created_at'], name='cmdlog_created_idx'),
        ),
        migrations.AddIndex(
            model_name='commandlog',
            index=models.Index(fields=['user', '-created_at'], name='cmdlog_user_created_idx'),
        ),
        migrations.AddIndex(
            model_name='commandlog',
            index=models.Index(fields=['status', '-created_at'], name='cmdlog_status_created_idx'),
        ),
        migrations.AddIndex(
            model_name='commandlog',
            index=models.Index(fields=['command', '-created_at'], name='cmdlog_cmd_created_idx'),
        ),
        migrations.AddIndex(
            model_name='loginlog',
            index=models.Index(fields=['-created_at'], name='loginlog_created_idx'),
        ),
        migrations.AddIndex(
            model_name='loginlog',
            index=models.Index(fields=['ip_address', '-created_at'], name='loginlog_ip_created_idx'),
        ),
        migrations.AddIndex(
            model_name='loginlog',
            index=models.Index(fields=['action', '-created_at'], name='loginlog_action_created_idx'),
        ),
        migrations.AddIndex(
            model_name='loginlog',
            index=models.Index(fields=['username', '-created_at'], name='loginlog_user_created_idx'),
        ),
    ]
