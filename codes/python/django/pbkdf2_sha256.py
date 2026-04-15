"""
pbkdf2_sha256.py

用途：
1. 生成与 Django `auth_user.password` 字段一致格式的密码哈希
2. 作为 Django 用户密码生成过程的本地备份说明文件

===============================================================================
一、Django 用户密码存储格式
===============================================================================

Django 使用 `pbkdf2_sha256` 时，最终存入数据库的字符串格式为：

    pbkdf2_sha256$iterations$salt$base64_hash

示例：

    pbkdf2_sha256$1000000$2K1TjzrMGAlCp6vhKvDQ5b$uc+H2x6eZUwKZ2Z/PjVI0hwIBZzZywT86tGh7y98daY=

4 段含义分别是：
- 算法名：pbkdf2_sha256
- 迭代次数：例如 1000000
- 盐值：随机字符串
- 哈希结果：PBKDF2-HMAC-SHA256 计算后的 base64 文本

===============================================================================
二、Django 用户密码的生成过程
===============================================================================

当 Django 执行：

    user.set_password("明文密码")

或内部调用：

    make_password("明文密码", hasher="pbkdf2_sha256")

时，核心流程如下：

1. 选择密码哈希器
   Django 根据 hasher='pbkdf2_sha256' 选择 PBKDF2PasswordHasher。

2. 生成随机盐值
   若未指定 salt，Django 会自动生成随机 salt。
   salt 的作用是：
   - 让相同明文密码得到不同哈希
   - 防止彩虹表攻击
   - 提高撞库/预计算攻击成本

3. 确定迭代次数
   Django 当前环境下的 pbkdf2_sha256 默认迭代次数由 Django 内部定义。
   当前项目环境实际输出可见为：
       1000000

4. 执行 PBKDF2-HMAC-SHA256
   本质相当于：

       hashlib.pbkdf2_hmac(
           'sha256',
           password.encode('utf-8'),
           salt.encode('utf-8'),
           iterations,
       )

   注意：
   - 这里不是简单 sha256(password)
   - 也不是简单 sha256(password + salt)
   - 而是标准 PBKDF2 算法，底层摘要函数为 HMAC-SHA256

5. 对结果做 base64 编码
   PBKDF2 输出的是二进制字节串，Django 会转为可存储文本。

6. 拼接最终字符串
   Django 最后将其拼成：

       pbkdf2_sha256$iterations$salt$base64_hash

===============================================================================
三、密码校验过程
===============================================================================

当 Django 执行：

    user.check_password("用户输入明文")

时，内部逻辑大致是：

1. 从数据库读取 password 字段
2. 解析出算法、iterations、salt、hash
3. 使用“用户输入明文 + 同样的 salt + 同样的 iterations”重新计算
4. 比较新结果和数据库中的 hash 是否一致
5. 若一致，则密码正确

===============================================================================
四、为什么这里直接调用 Django
===============================================================================

为了保证输出结果与当前 Django 环境完全一致，本脚本不手写主逻辑，
而是直接调用 Django 自带的：

    django.contrib.auth.hashers.make_password(...)

这样可以确保：
- 使用的是 Django 当前版本的真实 pbkdf2_sha256 实现
- 使用的是 Django 当前默认的迭代次数
- 输出格式与 auth_user.password 完全一致

===============================================================================
五、纯 Python 等价参考（仅作备份说明）
===============================================================================

下面的逻辑是 Django `pbkdf2_sha256` 的近似核心过程：

    import base64
    import hashlib

    password = "test123"
    salt = "random_salt"
    iterations = 1000000

    dk = hashlib.pbkdf2_hmac(
        'sha256',
        password.encode('utf-8'),
        salt.encode('utf-8'),
        iterations,
    )
    hash_value = base64.b64encode(dk).decode('ascii').strip()
    result = f"pbkdf2_sha256${iterations}${salt}${hash_value}"

这个过程仅用于理解原理。
本文件实际运行时仍以 Django 自带实现为准。
"""

import argparse
import base64
import getpass
import hashlib
import os
import sys

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'mysite.settings')

import django
from django.contrib.auth.hashers import make_password

django.setup()


def make_pbkdf2_sha256_hash_reference(password, salt, iterations):
    """
    纯 Python 参考实现，用于备份 Django pbkdf2_sha256 的核心计算过程。

    注意：
    1. 该函数的目标是帮助理解“Django 大致如何计算”
    2. 实际生产使用时，仍应优先调用 Django 自带的 make_password
    3. 返回格式与 Django password 字段一致：
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


def main():
    parser = argparse.ArgumentParser(
        description='Generate a Django pbkdf2_sha256 password hash'
    )
    parser.add_argument(
        'password',
        nargs='?',
        help='Plain text password. If omitted, you will be prompted securely.',
    )
    parser.add_argument(
        '--salt',
        help='Custom salt. If omitted, Django will generate a random salt.',
    )
    parser.add_argument(
        '--raw',
        action='store_true',
        help='Print only the hash result without any label',
    )
    args = parser.parse_args()

    password = args.password
    if password is None:
        password = getpass.getpass('请输入明文密码: ')

    if password == '':
        print('❌ 明文密码不能为空')
        sys.exit(1)

    # 这里直接调用 Django 自带的 make_password，
    # 以保证输出与当前 Django 环境中的 auth_user.password 格式完全一致。
    hash_result = make_password(
        password=password,
        salt=args.salt,
        hasher='pbkdf2_sha256',
    )

    if args.raw:
        print(hash_result)
    else:
        print('Django pbkdf2_sha256 hash:', hash_result)


if __name__ == '__main__':
    main()
