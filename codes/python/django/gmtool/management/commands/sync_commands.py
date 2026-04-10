"""管理命令：从idip_commands.json同步命令定义到数据库"""
from django.core.management.base import BaseCommand
from gmtool.command_parser import sync_commands_to_db


class Command(BaseCommand):
    help = '从idip_commands.json同步命令定义到数据库'

    def handle(self, *args, **options):
        created, updated, deactivated = sync_commands_to_db()
        self.stdout.write(self.style.SUCCESS(
            f'同步完成: 新增 {created} 条, 更新 {updated} 条, 停用 {deactivated} 条'
        ))
