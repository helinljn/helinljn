"""GM命令后台管理系统 - URL路由"""
from django.urls import path
from . import views

app_name = 'gmtool'

urlpatterns = [
    # 认证
    path('', views.dashboard, name='dashboard'),
    path('login/', views.login_view, name='login'),
    path('logout/', views.logout_view, name='logout'),

    # 命令管理
    path('commands/', views.command_list, name='command_list'),
    path('commands/add/', views.add_gm_command, name='add_gm_command'),
    path('commands/<str:cmd_id>/execute/', views.command_execute, name='command_execute'),

    # 用户管理
    path('users/', views.user_list, name='user_list'),
    path('users/create/', views.user_create, name='user_create'),
    path('users/<int:user_id>/edit/', views.user_edit, name='user_edit'),
    path('users/<int:user_id>/delete/', views.user_delete, name='user_delete'),

    # 角色管理
    path('roles/', views.role_list, name='role_list'),
    path('roles/create/', views.role_create, name='role_create'),
    path('roles/<int:role_id>/edit/', views.role_edit, name='role_edit'),
    path('roles/<int:role_id>/delete/', views.role_delete, name='role_delete'),

    # 用户权限分配
    path('users/<int:user_id>/permissions/', views.user_permissions, name='user_permissions'),

    # 操作日志
    path('logs/', views.command_log_list, name='command_log_list'),
    path('logs/login/', views.login_log_list, name='login_log_list'),

    # API
    path('api/commands/sync/', views.sync_commands_api, name='sync_commands_api'),
    path('api/commands/upload/', views.upload_commands_api, name='upload_commands_api'),
    path('api/logs/<int:log_id>/', views.log_detail_api, name='log_detail_api'),
]
