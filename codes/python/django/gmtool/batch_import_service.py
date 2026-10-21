"""批量命令导入解析服务。

支持 Excel「另存为文本」导出的制表符分隔文本（.txt/.tsv）以及 .csv。
仅依赖 Python 标准库，不引入额外第三方包。
"""
import codecs
import csv
import io
import json
import logging

from django.conf import settings
from django.utils.translation import gettext_lazy as _

from .command_services import validate_command_params_basic

logger = logging.getLogger(__name__)

ALLOWED_EXTENSIONS = ('.txt', '.tsv', '.csv')
DELIMITER_CANDIDATES = ('\t', ',', ';')
MAX_HEADER_SCAN_LINES = 50


class BatchImportError(Exception):
    """批量导入解析/校验错误。"""


class BatchRowResult:
    """单行解析结果。"""

    def __init__(self, row_index, params=None, error_message=''):
        self.row_index = row_index
        self.params = params
        self.error_message = error_message

    @property
    def is_valid(self):
        return not self.error_message


class BatchImportResult:
    """整份文件的解析结果。"""

    def __init__(self, command, headers, fields, unknown_headers, rows):
        self.command = command
        self.headers = headers
        self.fields = fields
        self.unknown_headers = unknown_headers
        self.rows = rows

    @property
    def total_rows(self):
        return len(self.rows)

    @property
    def valid_rows(self):
        return [row for row in self.rows if row.is_valid]

    @property
    def invalid_rows(self):
        return [row for row in self.rows if not row.is_valid]

    @property
    def is_valid(self):
        return bool(self.rows) and not self.invalid_rows


def get_max_import_rows():
    """单次导入允许的最大数据行数。"""
    default_max = getattr(settings, 'BATCH_EXECUTE_MAX_TARGETS', 100)
    return getattr(settings, 'BATCH_IMPORT_MAX_ROWS', default_max)


def validate_upload_filename(filename):
    """校验上传文件扩展名。"""
    lowered = (filename or '').lower()
    if not lowered.endswith(ALLOWED_EXTENSIONS):
        raise BatchImportError(
            _('仅支持 .txt、.tsv、.csv 文件，请将 Excel 表格另存为「文本文件(制表符分隔)」或 CSV')
        )


def validate_upload_size(size):
    """校验上传文件大小。"""
    max_size = getattr(settings, 'UPLOAD_MAX_SIZE', 10 * 1024 * 1024)
    if size > max_size:
        max_size_mb = max_size / (1024 * 1024)
        if max_size_mb.is_integer():
            max_size_display = str(int(max_size_mb))
        else:
            max_size_display = f'{max_size_mb:.2f}'.rstrip('0').rstrip('.')
        raise BatchImportError(
            _('文件大小不能超过 %(size)sMB') % {'size': max_size_display}
        )


def _decode_file_bytes(raw):
    """按 BOM / 常见编码探测解码文件内容。"""
    if raw.startswith(codecs.BOM_UTF8):
        return raw.decode('utf-8-sig')
    if raw.startswith(codecs.BOM_UTF16_LE) or raw.startswith(codecs.BOM_UTF16_BE):
        return raw.decode('utf-16')

    for encoding in ('utf-8', 'gb18030'):
        try:
            return raw.decode(encoding)
        except UnicodeDecodeError:
            continue

    raise BatchImportError(
        _('文件编码无法识别，请另存为 UTF-8 或 GBK 编码的文本文件')
    )


def _first_non_empty_lines(text, limit=MAX_HEADER_SCAN_LINES):
    lines = []
    for line in text.split('\n'):
        if line.strip():
            lines.append(line)
        if len(lines) >= limit:
            break
    return lines


def _detect_delimiter(text, filename):
    """根据扩展名与表头行内容确定分隔符。"""
    lowered = (filename or '').lower()
    if lowered.endswith('.csv'):
        candidates = (',', ';', '\t')
    else:
        candidates = ('\t', ',', ';')

    header_lines = _first_non_empty_lines(text)
    if not header_lines:
        return '\t'

    header_line = header_lines[0]
    for delimiter in candidates:
        if delimiter in header_line:
            return delimiter
    return '\t'


def read_tabular_rows(uploaded_file):
    """读取上传文件，返回 (headers, data_rows)。"""
    raw = uploaded_file.read()
    if not raw:
        raise BatchImportError(_('文件内容为空'))

    text = _decode_file_bytes(raw)
    text = text.replace('\r\n', '\n').replace('\r', '\n')
    delimiter = _detect_delimiter(text, getattr(uploaded_file, 'name', ''))

    reader = csv.reader(io.StringIO(text), delimiter=delimiter, quotechar='"')
    all_rows = []
    for row in reader:
        if not any(str(cell).strip() for cell in row):
            continue
        all_rows.append(row)

    if not all_rows:
        raise BatchImportError(_('文件内容为空'))

    headers = [str(cell).strip() for cell in all_rows[0]]
    if not any(headers):
        raise BatchImportError(_('未找到表头行'))

    data_rows = all_rows[1:]
    return headers, data_rows


def get_import_fields(command):
    """返回需要用户填写的请求字段（排除自动同步的 *_count 字段）。"""
    fields = []
    for field in (command.request_params or []):
        if not isinstance(field, dict):
            continue
        if field.get('auto_count_for'):
            continue
        if not field.get('id'):
            continue
        fields.append(field)
    return fields


def validate_headers(command, headers):
    """校验表头是否包含所有必填字段，返回 (fields, unknown_headers)。"""
    fields = get_import_fields(command)
    seen = set()
    duplicates = []
    for header in headers:
        if not header:
            continue
        if header in seen:
            duplicates.append(header)
        seen.add(header)

    if duplicates:
        raise BatchImportError(
            _('表头存在重复列：%(columns)s') % {'columns': ', '.join(sorted(set(duplicates)))}
        )

    required_ids = [
        field['id']
        for field in fields
        if str(field.get('isnull', 'false')).strip().lower() == 'false'
    ]
    missing = [field_id for field_id in required_ids if field_id not in seen]
    if missing:
        raise BatchImportError(
            _('缺少必填列：%(columns)s') % {'columns': ', '.join(missing)}
        )

    known_ids = {field['id'] for field in fields}
    unknown_headers = [header for header in headers if header and header not in known_ids]
    return fields, unknown_headers


def _normalize_time_cell(value):
    """兼容 Excel 常见日期写法，交给命令参数校验统一归一化。"""
    stripped = str(value).strip()
    if not stripped:
        return stripped
    if stripped.isdigit() or (stripped.startswith('-') and stripped[1:].isdigit()):
        return stripped

    normalized = stripped.replace('/', '-').replace('T', ' ')
    if len(normalized) == 16 and normalized.count(':') == 1:
        normalized = f'{normalized}:00'
    return normalized


def _parse_cell_value(field, raw_value):
    """按字段类型解析单元格，结构体列表要求单行 JSON。"""
    field_id = field['id']
    field_type = str(field.get('type', '')).strip().lower()
    children = field.get('children')

    if isinstance(children, list):
        try:
            parsed = json.loads(raw_value)
        except (TypeError, ValueError, json.JSONDecodeError):
            raise BatchImportError(
                _('字段 %(field)s 必须是合法的 JSON 数组') % {'field': field_id}
            )
        if not isinstance(parsed, list):
            raise BatchImportError(
                _('字段 %(field)s 必须是 JSON 数组') % {'field': field_id}
            )
        return parsed

    if field_type == 'time':
        return _normalize_time_cell(raw_value)
    return raw_value


def parse_rows(command, headers, data_rows):
    """逐行解析并校验，返回 BatchImportResult。"""
    fields, unknown_headers = validate_headers(command, headers)
    header_index = {}
    for index, header in enumerate(headers):
        if header and header not in header_index:
            header_index[header] = index

    max_rows = get_max_import_rows()
    if len(data_rows) > max_rows:
        raise BatchImportError(
            _('单次批量导入不能超过 %(n)s 行，当前 %(current)s 行')
            % {'n': max_rows, 'current': len(data_rows)}
        )

    results = []
    for row_index, raw_row in enumerate(data_rows, start=1):
        params = {}
        row_errors = []

        for field in fields:
            field_id = field['id']
            column = header_index.get(field_id)
            if column is None or column >= len(raw_row):
                continue
            raw_value = str(raw_row[column]).strip()
            if not raw_value:
                continue
            try:
                params[field_id] = _parse_cell_value(field, raw_value)
            except BatchImportError as exc:
                row_errors.append(str(exc))

        if row_errors:
            results.append(BatchRowResult(row_index, None, '；'.join(row_errors)))
            continue

        is_valid, error_message = validate_command_params_basic(command, params)
        if not is_valid:
            results.append(BatchRowResult(row_index, None, str(error_message)))
            continue

        results.append(BatchRowResult(row_index, params, ''))

    return BatchImportResult(command, headers, fields, unknown_headers, results)


def parse_uploaded_file(command, uploaded_file):
    """完整解析流程：扩展名/大小 → 读取表格 → 逐行校验。"""
    validate_upload_filename(getattr(uploaded_file, 'name', ''))
    validate_upload_size(getattr(uploaded_file, 'size', 0))
    headers, data_rows = read_tabular_rows(uploaded_file)
    if not data_rows:
        raise BatchImportError(_('文件中没有可导入的数据行'))
    return parse_rows(command, headers, data_rows)


def build_template_csv(command):
    """按命令请求字段生成 UTF-8 BOM CSV 模板内容（仅表头，避免示例数据被误执行）。"""
    fields = get_import_fields(command)
    buffer = io.StringIO()
    writer = csv.writer(buffer, delimiter=',', quoting=csv.QUOTE_MINIMAL, lineterminator='\r\n')
    writer.writerow([field['id'] for field in fields])
    return buffer.getvalue().encode('utf-8-sig')
