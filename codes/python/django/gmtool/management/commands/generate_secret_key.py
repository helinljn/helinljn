"""管理命令：生成 Django SECRET_KEY"""
import secrets
import string

from django.core.management.base import BaseCommand, CommandError


DEFAULT_LENGTH = 50
MIN_LENGTH = 32


def generate_django_secret_key(length=DEFAULT_LENGTH):
    """生成 Django SECRET_KEY。"""
    if length < MIN_LENGTH:
        raise ValueError(f'length must be at least {MIN_LENGTH}')

    characters = string.ascii_letters + string.digits + '!@#$%^&*(-_=+)'
    return ''.join(secrets.choice(characters) for _ in range(length))


class Command(BaseCommand):
    help = '生成 Django SECRET_KEY'

    def add_arguments(self, parser):
        parser.add_argument(
            '--length',
            type=int,
            default=DEFAULT_LENGTH,
            help=f'SECRET_KEY length (default: {DEFAULT_LENGTH}, minimum: {MIN_LENGTH})',
        )
        parser.add_argument(
            '--raw',
            action='store_true',
            help='仅输出密钥，不带提示文本',
        )

    def handle(self, *args, **options):
        try:
            secret_key = generate_django_secret_key(options['length'])
        except ValueError as e:
            raise CommandError(str(e)) from e

        if options['raw']:
            self.stdout.write(secret_key)
        else:
            self.stdout.write(f'Django SECRET_KEY: {secret_key}')
