from django.contrib import admin
from .models import GMCommand, Role, RoleCommandPermission, UserProfile, UserCommandPermission, CommandLog, LoginLog


@admin.register(GMCommand)
class GMCommandAdmin(admin.ModelAdmin):
    list_display = ('command_id', 'command_name', 'tab', 'request_name', 'is_active')
    list_filter = ('tab', 'is_active')
    search_fields = ('command_id', 'command_name', 'request_name')
    readonly_fields = ('command_id',)


@admin.register(Role)
class RoleAdmin(admin.ModelAdmin):
    list_display = ('name', 'display_name', 'is_super_admin', 'created_at')
    list_filter = ('is_super_admin',)


@admin.register(RoleCommandPermission)
class RoleCommandPermissionAdmin(admin.ModelAdmin):
    list_display = ('role', 'command')
    list_filter = ('role',)


@admin.register(UserCommandPermission)
class UserCommandPermissionAdmin(admin.ModelAdmin):
    list_display = ('user', 'command')
    list_filter = ('user',)
    search_fields = ('user__username', 'command__command_name')


@admin.register(UserProfile)
class UserProfileAdmin(admin.ModelAdmin):
    list_display = ('user', 'role', 'phone')
    list_filter = ('role',)


@admin.register(CommandLog)
class CommandLogAdmin(admin.ModelAdmin):
    list_display = ('user', 'command', 'partition', 'status', 'ip_address', 'created_at')
    list_filter = ('status', 'created_at')
    readonly_fields = ('user', 'command', 'partition', 'request_data', 'response_data', 'status', 'ip_address', 'created_at')


@admin.register(LoginLog)
class LoginLogAdmin(admin.ModelAdmin):
    list_display = ('username', 'action', 'ip_address', 'created_at')
    list_filter = ('action', 'created_at')
    search_fields = ('username', 'ip_address')
    readonly_fields = ('user', 'username', 'action', 'ip_address', 'user_agent', 'reason', 'created_at')
