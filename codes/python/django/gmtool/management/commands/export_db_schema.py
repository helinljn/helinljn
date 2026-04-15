"""管理命令：导出 SQLite 数据库表结构到 SQL 文件"""
import re
import sqlite3
from datetime import datetime

from django.core.management.base import BaseCommand, CommandError


def format_sql_statement(sql):
    """格式化 SQL 语句，使其更易读。"""
    if not sql:
        return sql

    sql = ' '.join(sql.split())

    if sql.upper().startswith('CREATE TABLE'):
        match = re.match(r'CREATE TABLE\s+"?(\w+)"?\s*\((.*)\)', sql, re.IGNORECASE)
        if match:
            table_name = match.group(1)
            columns_part = match.group(2)

            columns = []
            current = ''
            paren_depth = 0
            bracket_depth = 0

            for char in columns_part:
                if char == '(':
                    paren_depth += 1
                elif char == ')':
                    paren_depth -= 1
                elif char == '[':
                    bracket_depth += 1
                elif char == ']':
                    bracket_depth -= 1

                if char == ',' and paren_depth == 0 and bracket_depth == 0:
                    columns.append(current.strip())
                    current = ''
                else:
                    current += char

            if current:
                columns.append(current.strip())

            formatted_columns = []
            for col in columns:
                col_parts = col.split(' ', 1)
                if len(col_parts) == 2:
                    col_name = col_parts[0]
                    col_def = col_parts[1]
                    formatted_columns.append(f'  {col_name:30} {col_def}')
                else:
                    formatted_columns.append(f'  {col}')

            return f'CREATE TABLE "{table_name}" (\n' + ',\n'.join(formatted_columns) + '\n);'

    elif sql.upper().startswith('CREATE INDEX'):
        sql = re.sub(r'\s+ON\s+', '\n  ON ', sql, flags=re.IGNORECASE)
        sql = re.sub(r'\s+WHERE\s+', '\n  WHERE ', sql, flags=re.IGNORECASE)
        return sql + ';'

    elif sql.upper().startswith('CREATE UNIQUE INDEX'):
        sql = re.sub(r'\s+ON\s+', '\n  ON ', sql, flags=re.IGNORECASE)
        sql = re.sub(r'\s+WHERE\s+', '\n  WHERE ', sql, flags=re.IGNORECASE)
        return sql + ';'

    return sql + ';'


def export_schema_to_sql(database_path, output_path, format_output=True):
    """导出 SQLite 数据库的表结构到 SQL 文件。"""
    total_rows = 0

    with sqlite3.connect(database_path) as conn:
        cursor = conn.cursor()

        cursor.execute(
            "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name"
        )
        tables = cursor.fetchall()

        with open(output_path, 'w', encoding='utf-8') as f:
            f.write("-- ============================================================================\n")
            f.write("-- SQLite数据库表结构导出\n")
            f.write(f"-- 数据库文件: {database_path}\n")
            f.write(f"-- 生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"-- 总表数: {len(tables)}\n")
            f.write("-- ============================================================================\n\n")

            for i, table in enumerate(tables):
                table_name = table[0]

                cursor.execute(
                    "SELECT sql FROM sqlite_master WHERE type='table' AND name=?",
                    (table_name,),
                )
                row = cursor.fetchone()
                create_sql = row[0] if row else ''

                formatted_sql = (
                    format_sql_statement(create_sql)
                    if format_output
                    else (create_sql + ';' if create_sql else '')
                )

                f.write(f"-- {'=' * 76}\n")
                f.write(f"-- 表 {i + 1}: {table_name}\n")
                f.write(f"-- {'=' * 76}\n\n")

                if formatted_sql:
                    f.write(formatted_sql)
                    f.write('\n\n')

                cursor.execute(f'PRAGMA table_info("{table_name}")')
                columns = cursor.fetchall()

                f.write("-- 字段信息:\n")
                f.write(f"-- {'ID':<4} {'字段名':<20} {'类型':<15} {'非空':<6} {'主键':<6} {'默认值'}\n")
                f.write(f"-- {'-' * 4} {'-' * 20} {'-' * 15} {'-' * 6} {'-' * 6} {'-' * 20}\n")

                for col in columns:
                    col_id, col_name, col_type, notnull, dflt_value, pk = col
                    notnull_str = 'YES' if notnull else 'NO'
                    pk_str = 'YES' if pk else 'NO'
                    default_str = 'NULL' if dflt_value is None else str(dflt_value)
                    f.write(
                        f"-- {col_id:<4} {col_name:<20} {col_type:<15} {notnull_str:<6} {pk_str:<6} {default_str}\n"
                    )

                f.write('\n')

                cursor.execute(
                    "SELECT name, sql FROM sqlite_master WHERE type='index' AND tbl_name=? AND sql IS NOT NULL",
                    (table_name,),
                )
                indexes = cursor.fetchall()

                if indexes:
                    f.write("-- 索引:\n")
                    for idx_name, idx_sql in indexes:
                        formatted_idx = (
                            format_sql_statement(idx_sql)
                            if format_output
                            else (idx_sql + ';' if idx_sql else '')
                        )
                        if formatted_idx:
                            f.write(formatted_idx + '\n')
                    f.write('\n')

                try:
                    cursor.execute(f'SELECT COUNT(*) FROM "{table_name}"')
                    row_count = cursor.fetchone()[0]
                    f.write(f"-- 行数: {row_count}\n")
                except Exception:
                    f.write("-- 行数: 无法统计\n")

                f.write('\n\n')

            f.write("-- ============================================================================\n")
            f.write("-- 数据库统计信息\n")
            f.write("-- ============================================================================\n\n")

            for table in tables:
                table_name = table[0]
                try:
                    cursor.execute(f'SELECT COUNT(*) FROM "{table_name}"')
                    row_count = cursor.fetchone()[0]
                    f.write(f"-- {table_name:<30}: {row_count:>6} 行\n")
                    total_rows += row_count
                except Exception:
                    f.write(f"-- {table_name:<30}: 无法统计\n")

            f.write(f"\n-- 总行数: {total_rows}\n")
            f.write(f"-- 平均每表行数: {total_rows // len(tables) if len(tables) > 0 else 0}\n")

    return len(tables), total_rows


class Command(BaseCommand):
    help = '导出 SQLite 数据库表结构到 SQL 文件'

    def add_arguments(self, parser):
        parser.add_argument(
            '--db',
            default='db.sqlite3',
            help='数据库文件路径 (默认: db.sqlite3)',
        )
        parser.add_argument(
            '--output',
            default='database_schema.sql',
            help='输出文件路径 (默认: database_schema.sql)',
        )
        parser.add_argument(
            '--no-format',
            action='store_true',
            dest='no_format',
            help='禁用 SQL 格式化，输出原始单行 SQL',
        )

    def handle(self, *args, **options):
        try:
            table_count, total_rows = export_schema_to_sql(
                options['db'],
                options['output'],
                format_output=not options['no_format'],
            )
        except Exception as e:
            raise CommandError(f'导出失败: {e}') from e

        self.stdout.write(self.style.SUCCESS(f'数据库结构已导出到: {options["output"]}'))
        self.stdout.write(self.style.SUCCESS(f'共导出 {table_count} 个表'))
        self.stdout.write(self.style.SUCCESS(f'总行数: {total_rows}'))
        if options['no_format']:
            self.stdout.write(self.style.SUCCESS('文件已按原始 SQL 导出'))
        else:
            self.stdout.write(self.style.SUCCESS('文件已格式化，便于阅读'))
