# Regen 📋

![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)

## 简介 / Introduction

Regen 是一个命令行工具，用于快速生成正则表达式。它提供两种模式：
- 直接参数模式：通过命令行参数快速生成
- 向导模式：交互式构建复杂正则表达式

Regen is a command-line tool for quickly generating regular expressions. It offers two modes:
- Direct mode: Quick generation via command-line arguments
- Wizard mode: Interactive building of complex regular expressions

## 功能特性 / Features

### 预设模式 / Preset Patterns
- 电子邮件 / Email
- URL
- IP地址 / IP Address
- 日期 / Date
- 时间 / Time
- 电话号码 / Phone Number
- 十六进制颜色 / Hex Color
- 信用卡号 / Credit Card

### 自定义选项 / Customization Options
- 整行匹配 / Whole line matching
- 不区分大小写 / Case insensitive
- 多行模式 / Multiline mode
- Dot匹配所有字符 / Dot matches all
- 输出解释 / Explanation output
- 多种语言代码生成 / Multiple language code generation

## 安装 / Installation

### 从源码编译 / Build from source

```bash
git clone https://github.com/safe049/regen.git
cd regen
g++ -std=c++11 regen.cpp -o regen
sudo mv regen /usr/local/bin/
```

## 使用示例 / Usage Examples

### 直接模式 / Direct Mode

```bash
# 使用预设模式 / Use preset pattern
regen -p email

# 自定义模式 / Custom pattern
regen "\d+" "匹配一个或多个数字 / Match one or more digits"

# 输出解释 / With explanation
regen -e -p ip

# 输出为Python代码 / Output as Python code
regen -f python -p date
```

### 向导模式 / Wizard Mode

```bash
regen --wizard
```

## 许可证 / License

GNU General Public License v3.0

## 贡献 / Contributing

欢迎提交问题和拉取请求！
Issues and pull requests are welcome!

## 作者 / Author

Dysprosium <safe049@163.com>
