import argparse
import secrets
import string


DEFAULT_LENGTH = 50
MIN_LENGTH = 32


def generate_django_secret_key(length=DEFAULT_LENGTH):
    """
    生成 Django SECRET_KEY。

    Args:
        length: 密钥长度，建议不低于 32。

    Returns:
        生成的 SECRET_KEY 字符串。
    """
    if length < MIN_LENGTH:
        raise ValueError(f"length must be at least {MIN_LENGTH}")

    # 定义字符集：大小写字母 + 数字 + 特殊符号
    characters = string.ascii_letters + string.digits + '!@#$%^&*(-_=+)'
    # 使用加密安全的随机生成器
    return ''.join(secrets.choice(characters) for _ in range(length))


def main():
    parser = argparse.ArgumentParser(description='Generate a Django SECRET_KEY')
    parser.add_argument(
        '--length',
        type=int,
        default=DEFAULT_LENGTH,
        help=f'SECRET_KEY length (default: {DEFAULT_LENGTH}, minimum: {MIN_LENGTH})',
    )
    parser.add_argument(
        '--raw',
        action='store_true',
        help='Print only the key without any label',
    )
    args = parser.parse_args()

    secret_key = generate_django_secret_key(args.length)

    if args.raw:
        print(secret_key)
    else:
        print("Django SECRET_KEY:", secret_key)


if __name__ == '__main__':
    main()
