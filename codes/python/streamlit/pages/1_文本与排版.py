"""
模块1：文本与排版
本页面展示 Streamlit 所有文本展示相关函数，
包括标题、普通文本、Markdown、代码、公式等。

本模块适合初学者理解如何在页面中优雅展示各种文本内容。
"""

import streamlit as st

st.set_page_config(
    page_title="文本与排版 - Streamlit 学习",
    page_icon="📝",
    layout="wide"
)

st.title("📝 模块1：文本与排版")

st.markdown(
    """
### 简介
文本展示是网页设计的基础。Streamlit 支持多种文本展示方式，
包含标题、普通文本、Markdown、代码块、公式等，
方便开发者灵活展示内容。

下面将依次演示这些文本展示控件的用法及效果。
"""
)

st.divider()

# 标题层级展示
st.header("标题层级")
st.subheader("st.title() - 最高级标题")
st.title("这是一个 Title 级别的大标题")

st.subheader("st.header() - 一级标题")
st.header("这是一个 Header 级别的标题")

st.subheader("st.subheader() - 二级标题")
st.subheader("这是一个 Subheader 级别的标题")

st.caption("caption() 用于展示说明性文字，通常比正文稍小且颜色浅。")
st.caption("示例: 这是一个辅助说明文字，用于注释或提醒")

st.divider()

# 普通文本与 Markdown
st.header("文本和 Markdown")

st.subheader("st.text() - 纯文本，等宽字体")
st.text("这是用 st.text() 显示的文本，等宽字体，不支持格式。")

st.subheader("st.markdown() - 支持 Markdown 格式的文本展示")
markdown_text = """
# 这是 Markdown 的 H1 标题
## 这是 Markdown 的 H2 标题

**加粗文本**，*斜体文本*，~~删除线~~，以及 [链接](https://streamlit.io) 。

- 项目1
- 项目2
- 项目3

> 这是一段引用文字。

代码示例：

```python
def hello():
    print("Hello, Streamlit!")
```
"""
st.markdown(markdown_text)

st.divider()

# 代码块输出
st.header("代码块展示")

st.subheader("st.code()")
code_string = '''import streamlit as st

st.write("显示代码和执行结果")'''
st.code(code_string, language="python")

st.divider()

# 公式展示
st.header("数学公式展示")

latex_formula = r'''
a^2 + b^2 = c^2
\int_0^\infty e^{-x} dx = 1
\sum_{i=1}^n i = \frac{n(n+1)}{2}
'''

st.latex(latex_formula)

st.divider()

# 万能输出 st.write
st.header("万能输出函数 st.write")

st.write("你可以用 st.write 输出各种类型的内容:")
st.write("字符串", 123, 45.6)
st.write({"key": "value", "nums": [1, 2, 3]})
st.write([1, 2, 3, 4, 5])
st.write(st.session_state)

st.divider()

# 演示 st.echo() — 显示源码并执行
st.header("st.echo() 示例 — 显示并执行代码")

st.markdown("下面的代码块会显示它的源码和运行结果，适合教学使用。")

with st.echo():
    st.write("这是 st.echo() 里的内容，会显示代码且运行。")
    x = 42
    st.write(f"x 的值是 {x}")

st.divider()

# 分割线展示
st.header("分割线 — st.divider()")
st.markdown("页面中用分割线划分区域，提升可读性。")

st.divider()
st.write("这是分割线下面的内容。")

st.divider()

# 自定义 HTML 渲染
st.header("自定义 HTML 显示")

html_string = """
<div style="border:2px solid red; padding: 10px; border-radius: 12px;">
  <h3 style="color: purple;">自定义 HTML 示例</h3>
  <p>你可以用 <code>st.markdown</code> 的 <code>unsafe_allow_html=True</code> 参数显示任意 HTML 内容。</p>
</div>
"""

st.markdown(html_string, unsafe_allow_html=True)

st.markdown(
    """
---

> 注意：
>
> - 自定义 HTML 可能存在安全风险，确保内容可信。
> - 部分 CSS 和 JS 可能被浏览器或 Streamlit 限制。
"""
)
