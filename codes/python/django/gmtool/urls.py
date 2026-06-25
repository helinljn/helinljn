"""GM命令后台管理系统 - URL路由"""
from django.urls import path

from . import announcement_views, api_views, auth_views, command_views, user_views

app_name = 'gmtool'

urlpatterns = [
    # 认证
    path('', command_views.dashboard, name='dashboard'),
    path('login/', auth_views.login_view, name='login'),
    path('logout/', auth_views.logout_view, name='logout'),

    # 命令管理
    path('commands/', command_views.command_list, name='command_list'),
    path('commands/add/', command_views.add_gm_command, name='add_gm_command'),
    path('commands/<str:cmd_id>/execute/', command_views.command_execute, name='command_execute'),

    # 用户管理
    path('users/', user_views.user_list, name='user_list'),
    path('users/create/', user_views.user_create, name='user_create'),
    path('users/<int:user_id>/edit/', user_views.user_edit, name='user_edit'),
    path('users/<int:user_id>/delete/', user_views.user_delete, name='user_delete'),
    path('users/<int:user_id>/permissions/', user_views.user_permissions, name='user_permissions'),

    # 操作日志
    path('logs/', command_views.command_log_list, name='command_log_list'),
    path('logs/login/', user_views.login_log_list, name='login_log_list'),

    # 公告管理
    path('announcements/', announcement_views.announcement_list, name='announcement_list'),
    path('announcements/query/', announcement_views.announcement_query, name='announcement_query'),
    path('announcements/create/', announcement_views.announcement_create, name='announcement_create'),
    path('announcements/batch-delete/', announcement_views.announcement_batch_delete, name='announcement_batch_delete'),
    path('announcements/logs/', announcement_views.announcement_log_list, name='announcement_log_list'),

    # API v1
    path('api/v1/commands/sync/', command_views.sync_commands_api, name='sync_commands_api'),
    path('api/v1/commands/upload/', api_views.upload_commands_api, name='upload_commands_api'),
    path('api/v1/logs/<int:log_id>/', api_views.log_detail_api, name='log_detail_api'),
    path('api/v1/announcements/logs/<int:log_id>/', api_views.announcement_log_detail_api, name='announcement_log_detail_api'),
]
