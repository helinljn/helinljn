"""管理命令：生成 Django pbkdf2_sha256 密码哈希"""
import base64
import getpass
import hashlib

from django.contrib.auth.hashers import make_password
from django.core.management.base import BaseCommand, CommandError


def make_pbkdf2_sha256_hash_reference(password, salt, iterations):
    """
    纯 Python 参考实现，用于备份 Django pbkdf2_sha256 的核心计算过程。
    返回格式与 Django password 字段一致：
    pbkdf2_sha256$iterations$salt$base64_hash
    """
    if password is None or password == '':
        raise ValueError('password cannot be empty')
    if not salt:
        raise ValueError('salt cannot be empty')
    if iterations <= 0:
        raise ValueError('iterations must be greater than 0')

    dk = hashlib.pbkdf2_hmac(
        'sha256',
        password.encode('utf-8'),
        salt.encode('utf-8'),
        iterations,
    )
    hash_value = base64.b64encode(dk).decode('ascii').strip()
    return f'pbkdf2_sha256${iterations}${salt}${hash_value}'


class Command(BaseCommand):
    help = '生成 Django pbkdf2_sha256 密码哈希'

    def add_arguments(self, parser):
        parser.add_argument(
            'password',
            nargs='?',
            help='明文密码；不传则进入安全输入模式',
        )
        parser.add_argument(
            '--salt',
            help='自定义 salt；不传则由 Django 自动生成随机 salt',
        )
        parser.add_argument(
            '--raw',
            action='store_true',
            help='仅输出哈希结果，不带提示文本',
        )

    def handle(self, *args, **options):
        password = options.get('password')
        if password is None:
            password = getpass.getpass('请输入明文密码: ')

        if password == '':
            raise CommandError('明文密码不能为空')

        hash_result = make_password(
            password=password,
            salt=options.get('salt'),
            hasher='pbkdf2_sha256',
        )

        if options['raw']:
            self.stdout.write(hash_result)
        else:
            self.stdout.write(f'Django pbkdf2_sha256 hash: {hash_result}')
