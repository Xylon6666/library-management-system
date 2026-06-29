# library-management-system
![C](https://img.shields.io.cn/badge/language-C-blue)
![Platform](https://img.shields.io.cn/badge/platform-Windows-lightgrey)
![License](https://img.shields.io.cn/badge/license-MIT-green)
简单图书管理系统 —— C语言 + 单向链表 + 文件持久化，支持超级管理员/普通管理员/读者三角色

## 功能

- **图书管理**：采编入库、清除库存、多条件检索（书号/书名/作者/出版社）
- **借阅管理**：借书、还书、续借、逾期罚金自动计算
- **用户管理**：读者账户创建/删除/查询，普通管理员账户管理（超管权限）
- **数据统计**：藏书总量、借阅排行、罚金排行、月度统计
- **数据持久化**：通过文本文件（books.txt / users.dat / borrow.txt）保存数据，程序重启后数据不丢失
- **交互优化**：控制台彩色输出、自定义暂停提示、错误码详细显示

## 环境要求

- **操作系统**：Windows（依赖 windows.h / SetConsoleTextAttribute / _kbhit 等 Win32 API）
- **编译器**：MinGW（GCC for Windows）或 Visual Studio（MSVC）
- **编码**：源文件为 UTF-8，运行时控制台需支持 GBK（程序内已做 _setmode 处理）

## 编译运行

### 方法一：命令行（MinGW）

```bash

gcc *.c -o library.exe -Wall

library.exe

```

### 方法二：Visual Studio

1. 新建控制台应用程序项目
2. 将所有 `.c` 和 `.h` 文件添加到源文件/头文件目录
3. 按 F5 编译运行

## 快速开始

1. 首次运行程序会自动检测 `data.dat` 是否存在，若不存在则进入首次初始化流程。
2. 初始化会创建默认超级管理员账户（账号：admin，密码：admin123）。
3. 登录后可按菜单提示操作，图书和借阅数据文件会在程序退出时自动保存。

## 项目结构
├── Common.h          # 公共头文件、结构体定义、宏常量

├── Common.c          # 工具函数（颜色设置、错误提示、暂停）

├── Main.c            # 程序入口、主菜单循环

├── Auth.c            # 身份验证模块

├── Menu.c            # 菜单界面模块

├── Business.c        # 核心业务模块（图书/借阅/用户管理）

├── Personal.c        # 读者个人中心（借阅记录、罚金、续借）

├── Statistic.c       # 统计分析模块

├── Data.c            # 数据持久化模块（文件读写）

├── Library.h         # 函数接口声明

├── data/             # 示例数据文件（程序首次运行也会自动生成）

│   ├── books.txt

│   ├── users.dat

│   ├── borrow.txt

│   └── data.dat

└── README.md

## 注意事项

- 本项目仅支持 **Windows** 平台，Linux/macOS 下无法编译。
- 若运行后中文显示乱码，请确保控制台代码页为 GBK（右键标题栏 → 属性 → 字体 → 勾选“使用旧版控制台”）。
- 数据文件位于程序所在目录，请勿手动修改格式，否则可能导致加载失败。

## License
MIT © 2025 
