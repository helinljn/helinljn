import sqlite3
import sys
import re
from datetime import datetime


def format_sql_statement(sql):
    """
    格式化SQL语句，使其更易读
    将CREATE TABLE和CREATE INDEX语句格式化为多行
    """
    if not sql:
        return sql

    # 移除多余的空格和换行
    sql = ' '.join(sql.split())

    # 处理CREATE TABLE语句
    if sql.upper().startswith('CREATE TABLE'):
        # 提取表名和字段部分
        match = re.match(r'CREATE TABLE\s+"?(\w+)"?\s*\((.*)\)', sql, re.IGNORECASE)
        if match:
            table_name = match.group(1)
            columns_part = match.group(2)

            # 分割字段，但避免分割CHECK约束内的逗号
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

            # 重新构建格式化的CREATE TABLE语句
            formatted_columns = []
            for col in columns:
                # 美化每个字段
                col_parts = col.split(' ', 1)
                if len(col_parts) == 2:
                    col_name = col_parts[0]
                    col_def = col_parts[1]
                    # 对齐类型和约束
                    formatted_columns.append(f'  {col_name:30} {col_def}')
                else:
                    formatted_columns.append(f'  {col}')

            return f'CREATE TABLE "{table_name}" (\n' + ',\n'.join(formatted_columns) + '\n);'

    # 处理CREATE INDEX语句
    elif sql.upper().startswith('CREATE INDEX'):
        # 简单地在ON和WHERE前添加换行
        sql = re.sub(r'\s+ON\s+', '\n  ON ', sql, flags=re.IGNORECASE)
        sql = re.sub(r'\s+WHERE\s+', '\n  WHERE ', sql, flags=re.IGNORECASE)
        return sql + ';'

    # 处理CREATE UNIQUE INDEX语句
    elif sql.upper().startswith('CREATE UNIQUE INDEX'):
        sql = re.sub(r'\s+ON\s+', '\n  ON ', sql, flags=re.IGNORECASE)
        sql = re.sub(r'\s+WHERE\s+', '\n  WHERE ', sql, flags=re.IGNORECASE)
        return sql + ';'

    return sql + ';'


def export_schema_to_sql(database_path, output_path, format_output=True):
    """
    导出SQLite数据库的表结构到SQL文件，格式美观易读
    """
    total_rows = 0

    with sqlite3.connect(database_path) as conn:
        cursor = conn.cursor()

        # 获取所有表
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name")
        tables = cursor.fetchall()

        with open(output_path, 'w', encoding='utf-8') as f:
            # 文件头部信息
            f.write("-- ============================================================================\n")
            f.write("-- SQLite数据库表结构导出\n")
            f.write(f"-- 数据库文件: {database_path}\n")
            f.write(f"-- 生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"-- 总表数: {len(tables)}\n")
            f.write("-- ============================================================================\n\n")

            # 导出每个表的创建语句
            for i, table in enumerate(tables):
                table_name = table[0]

                # 获取创建表的SQL语句
                cursor.execute("SELECT sql FROM sqlite_master WHERE type='table' AND name=?", (table_name,))
                row = cursor.fetchone()
                create_sql = row[0] if row else ''

                # 格式化SQL语句
                formatted_sql = format_sql_statement(create_sql) if format_output else (create_sql + ';' if create_sql else '')

                # 表头
                f.write(f"-- {'=' * 76}\n")
                f.write(f"-- 表 {i+1}: {table_name}\n")
                f.write(f"-- {'=' * 76}\n\n")

                # 输出格式化的CREATE TABLE语句
                if formatted_sql:
                    f.write(formatted_sql)
                    f.write('\n\n')

                # 获取表的字段信息
                cursor.execute(f'PRAGMA table_info("{table_name}")')
                columns = cursor.fetchall()

                # 输出字段信息
                f.write("-- 字段信息:\n")
                f.write(f"-- {'ID':<4} {'字段名':<20} {'类型':<15} {'非空':<6} {'主键':<6} {'默认值'}\n")
                f.write(f"-- {'-' * 4} {'-' * 20} {'-' * 15} {'-' * 6} {'-' * 6} {'-' * 20}\n")

                for col in columns:
                    col_id, col_name, col_type, notnull, dflt_value, pk = col
                    notnull_str = 'YES' if notnull else 'NO'
                    pk_str = 'YES' if pk else 'NO'
                    default_str = 'NULL' if dflt_value is None else str(dflt_value)
                    f.write(f"-- {col_id:<4} {col_name:<20} {col_type:<15} {notnull_str:<6} {pk_str:<6} {default_str}\n")

                f.write('\n')

                # 获取索引信息
                cursor.execute("SELECT name, sql FROM sqlite_master WHERE type='index' AND tbl_name=? AND sql IS NOT NULL", (table_name,))
                indexes = cursor.fetchall()

                if indexes:
                    f.write("-- 索引:\n")
                    for idx_name, idx_sql in indexes:
                        formatted_idx = format_sql_statement(idx_sql) if format_output else (idx_sql + ';' if idx_sql else '')
                        if formatted_idx:
                            f.write(formatted_idx + '\n')
                    f.write('\n')

                # 获取表行数
                try:
                    cursor.execute(f'SELECT COUNT(*) FROM "{table_name}"')
                    row_count = cursor.fetchone()[0]
                    f.write(f"-- 行数: {row_count}\n")
                except Exception:
                    f.write("-- 行数: 无法统计\n")

                f.write('\n\n')

            # 数据库统计信息
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

    print(f"✅ 数据库结构已导出到: {output_path}")
    print(f"📊 共导出 {len(tables)} 个表")
    print(f"📈 总行数: {total_rows}")
    print("📝 文件已格式化，便于阅读" if format_output else "📝 文件已按原始 SQL 导出")


def main():
    """
    主函数：解析命令行参数并执行导出
    """
    import argparse

    parser = argparse.ArgumentParser(description='导出SQLite数据库表结构到格式化的SQL文件')
    parser.add_argument('--db', default='db.sqlite3', help='数据库文件路径 (默认: db.sqlite3)')
    parser.add_argument('--output', default='database_schema.sql', help='输出文件路径 (默认: database_schema.sql)')
    parser.add_argument('--no-format', action='store_true', dest='no_format', help='禁用SQL格式化，输出原始单行SQL')

    args = parser.parse_args()

    try:
        export_schema_to_sql(args.db, args.output, format_output=not args.no_format)
    except Exception as e:
        print(f"❌ 导出失败: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
