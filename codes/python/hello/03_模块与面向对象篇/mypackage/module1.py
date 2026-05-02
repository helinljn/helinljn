"""
mypackage.module1 - 字符串工具模块

演示包中子模块的编写方式。
"""


def greet(name: str, language: str = "zh") -> str:
    """多语言问候函数。

    Args:
        name: 要问候的人名
        language: 语言代码（zh/en/ja）

    Returns:
        问候语字符串
    """
    greetings = {
        "zh": f"你好，{name}！",
        "en": f"Hello, {name}!",
        "ja": f"こんにちは、{name}！",
        "fr": f"Bonjour, {name}!",
    }
    return greetings.get(language, greetings["en"])


def reverse_string(s: str) -> str:
    """反转字符串。"""
    return s[::-1]


def count_words(text: str) -> int:
    """统计文本中的单词数量。"""
    return len(text.split())


# 模块私有函数
def _normalize(text: str) -> str:
    """内部使用的文本规范化函数。"""
    return text.strip().lower()


# 测试代码
if __name__ == "__main__":
    print("module1 直接运行测试：")
    print(greet("World"))
    print(greet("世界", "zh"))
    print(reverse_string("Hello, Python!"))
    print(f"单词数: {count_words('Python is awesome')}")
