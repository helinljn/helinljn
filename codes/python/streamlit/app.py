#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import streamlit as st

# 预设的账号信息（在实际应用中，应使用更安全的方式存储密码，如加密）
usernames_passwords = {
    "test": "111111"
}

# 登录表单函数
def login_form():
    st.title("登录页面")
    username = st.text_input("用户名")
    password = st.text_input("密码", type="password")

    if st.button("登录"):
        # 验证用户输入
        if username in usernames_passwords and usernames_passwords[username] == password:
            st.success("登录成功！")
            # 这里可以跳转至应用主页或展示其他内容
        else:
            st.error("用户名或密码错误，请重试。")

# 主函数
if __name__ == "__main__":
    login_form()