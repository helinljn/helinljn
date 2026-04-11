"""Django Admin 后台管理注册"""
from django.contrib import admin
from .models import GMCommand, Role, UserProfile, UserCommandPermission, CommandLog, LoginLog


@admin.register(GMCommand)
class GMCommandAdmin(admin.ModelAdmin):
    list_display = ['command_id', 'command_name', 'tab', 'request_name', 'is_active']
    list_filter = ['is_active', 'tab']
    search_fields = ['command_id', 'command_name']
    readonly_fields = ['request_params', 'response_params']


@admin.register(Role)
class RoleAdmin(admin.ModelAdmin):
    list_display = ['name', 'display_name', 'is_super_admin', 'created_at']
    list_filter = ['is_super_admin']
    search_fields = ['name', 'display_name']


@admin.register(UserProfile)
class UserProfileAdmin(admin.ModelAdmin):
    list_display = ['user', 'role', 'phone']
    list_filter = ['role']
    search_fields = ['user__username']
    raw_id_fields = ['user']


@admin.register(UserCommandPermission)
class UserCommandPermissionAdmin(admin.ModelAdmin):
    list_display = ['user', 'command']
    list_filter = ['command__tab']
    search_fields = ['user__username', 'command__command_name']
    raw_id_fields = ['user', 'command']


@admin.register(CommandLog)
class CommandLogAdmin(admin.ModelAdmin):
    list_display = ['user', 'command', 'partition', 'status', 'ip_address', 'created_at']
    list_filter = ['status']
    search_fields = ['user__username', 'command__command_name']
    readonly_fields = ['request_data', 'request_content', 'response_data']
    raw_id_fields = ['user', 'command']


@admin.register(LoginLog)
class LoginLogAdmin(admin.ModelAdmin):
    list_display = ['username', 'action', 'ip_address', 'created_at']
    list_filter = ['action']
    search_fields = ['username']
    readonly_fields = ['user_agent', 'reason']
