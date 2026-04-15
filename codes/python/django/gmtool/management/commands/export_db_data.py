"""管理命令：导出 SQLite 当前数据到 SQL 文件（仅数据）"""
import sqlite3
from datetime import datetime

from django.core.management.base import BaseCommand, CommandError


def quote_sql_value(value):
    """将 Python 值转换为可直接写入 SQLite SQL 的字面量。"""
    if value is None:
        return 'NULL'
    if isinstance(value, bool):
        return '1' if value else '0'
    if isinstance(value, (int, float)):
        return str(value)
    if isinstance(value, bytes):
        return "X'{}'".format(value.hex())

    text = str(value).replace("'", "''")
    return f"'{text}'"


def get_user_tables(cursor):
    """获取用户表（排除 sqlite 内部表）。"""
    cursor.execute("""
        SELECT name
        FROM sqlite_master
        WHERE type = 'table'
          AND name NOT LIKE 'sqlite_%'
        ORDER BY name
    """)
    return [row[0] for row in cursor.fetchall()]


def export_table_data(cursor, table_name, file_obj):
    """导出单表数据为 DELETE + INSERT 语句。"""
    cursor.execute(f'PRAGMA table_info("{table_name}")')
    columns_info = cursor.fetchall()
    column_names = [col[1] for col in columns_info]

    if not column_names:
        return 0

    quoted_columns = ', '.join(f'"{name}"' for name in column_names)
    cursor.execute(f'SELECT {quoted_columns} FROM "{table_name}"')
    rows = cursor.fetchall()

    file_obj.write(f"-- {'=' * 76}\n")
    file_obj.write(f"-- 表数据: {table_name}\n")
    file_obj.write(f"-- 行数: {len(rows)}\n")
    file_obj.write(f"-- {'=' * 76}\n\n")

    file_obj.write(f'DELETE FROM "{table_name}";\n')

    for row in rows:
        values_sql = ', '.join(quote_sql_value(value) for value in row)
        file_obj.write(
            f'INSERT INTO "{table_name}" ({quoted_columns}) VALUES ({values_sql});\n'
        )

    file_obj.write('\n')
    return len(rows)


def export_data_to_sql(database_path, output_path, tables=None):
    """导出 SQLite 当前数据到 SQL 文件（仅数据，不含建表语句）。"""
    total_rows = 0

    with sqlite3.connect(database_path) as conn:
        cursor = conn.cursor()
        all_tables = get_user_tables(cursor)

        if tables:
            table_set = set(tables)
            selected_tables = [table for table in all_tables if table in table_set]
            missing_tables = [table for table in tables if table not in all_tables]
            if missing_tables:
                raise ValueError(f'以下表不存在: {", ".join(missing_tables)}')
        else:
            selected_tables = all_tables

        with open(output_path, 'w', encoding='utf-8') as f:
            f.write("-- ============================================================================\n")
            f.write("-- SQLite 数据导出文件（仅数据）\n")
            f.write(f"-- 数据库文件: {database_path}\n")
            f.write(f"-- 生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"-- 导出表数: {len(selected_tables)}\n")
            f.write("-- ============================================================================\n\n")

            f.write("BEGIN TRANSACTION;\n\n")

            for table_name in selected_tables:
                total_rows += export_table_data(cursor, table_name, f)

            f.write("COMMIT;\n")

    return len(selected_tables), total_rows


class Command(BaseCommand):
    help = '导出 SQLite 当前数据到 SQL 文件（仅数据）'

    def add_arguments(self, parser):
        parser.add_argument(
            '--db',
            default='db.sqlite3',
            help='数据库文件路径 (默认: db.sqlite3)',
        )
        parser.add_argument(
            '--output',
            default='database_data.sql',
            help='输出文件路径 (默认: database_data.sql)',
        )
        parser.add_argument(
            '--tables',
            nargs='*',
            help='仅导出指定表，多个表用空格分隔，例如: --tables auth_user gmtool_gmcommand',
        )

    def handle(self, *args, **options):
        try:
            table_count, total_rows = export_data_to_sql(
                options['db'],
                options['output'],
                tables=options.get('tables'),
            )
        except Exception as e:
            raise CommandError(f'导出失败: {e}') from e

        self.stdout.write(self.style.SUCCESS(f'数据已导出到: {options["output"]}'))
        self.stdout.write(self.style.SUCCESS(f'共导出 {table_count} 个表'))
        self.stdout.write(self.style.SUCCESS(f'总行数: {total_rows}'))
