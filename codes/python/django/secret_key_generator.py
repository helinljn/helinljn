import secrets
import string

def generate_django_secret_key(length=50):
    # 定义字符集：大小写字母 + 数字 + 特殊符号
    characters = string.ascii_letters + string.digits + '!@#$%^&*(-_=+)'
    # 使用加密安全的随机生成器
    return ''.join(secrets.choice(characters) for _ in range(length))

secret_key = generate_django_secret_key()

print("Django SECRET_KEY:", secret_key, end='')
