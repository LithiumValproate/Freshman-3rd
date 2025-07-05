# Qt-Web 学生管理系统

<div align="center">

![Language](https://img.shields.io/badge/Languages-C++%20|%20Vue%20|%20Go%20|%20Python-blue)
![Framework](https://img.shields.io/badge/Framework-Qt%20|%20Vue.js-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

</div>

## 📝 项目概述

Qt-Web 学生管理系统是一个创新性融合项目，将现代 Web 前端与 Qt 桌面应用无缝集成，为学生信息管理提供跨平台解决方案。本系统兼具 Web 应用的灵活性与原生桌面应用的高效性，实现了技术栈的优势互补。

## ✨ 核心特性

- **桌面 Web 融合**：使用 QWebEngineView 嵌入 Vue 前端，提供原生桌面窗口体验
- **双向通信机制**：基于 QWebChannel 实现 Web 前端与 Qt 后端的高效实时交互
- **数据安全保障**：自动备份、恢复与设置持久化功能
- **现代化 UI/UX**：响应式布局设计，状态栏集成，进度条反馈，全屏支持
- **系统深度集成**：系统托盘、全局快捷键、原生文件对话框、系统通知服务

## 🔧 技术栈详解

### 前端技术
- **Vue.js** (55.5%): 构建响应式用户界面
- **JavaScript**: 客户端逻辑处理

### 后端技术
- **C++** (29.7%): Qt 框架核心逻辑
- **Go** (5.2%): 辅助服务和微服务支持
- **Rust** (1.6%): 性能关键模块
- **CMake** (3.3%): 跨平台构建系统

### 核心组件
- **Qt 5/6**: 应用程序框架和 UI 渲染
- **QWebEngine**: Web 内容渲染引擎
- **QWebChannel**: 前后端双向通信桥接

## 🚀 快速开始

### 环境准备
- Qt 5/6 开发环境
- C++ 编译器
- Node.js 和 npm (用于 Web 前端)
- Go 编译器 (可选)
- Rust 编译器 (可选)

### 获取代码
```bash
git clone https://github.com/LithiumValproate/Freshman-3rd.git
cd Freshman-3rd
```

### 构建与运行

#### Qt 桌面端
使用 Qt Creator 打开项目，或通过命令行：

```bash
# 使用 qmake
qmake && make
./bin/QtWebStudentSys

# 或使用 CMake
cmake .
make
./bin/QtWebStudentSys
```

#### Web 前端 (独立开发模式)
```bash
cd web/vue-proj
npm install
npm run dev
```

## 📂 项目结构

```
Freshman-3rd/
├── main.cpp                # 程序入口
├── mainwindow.*            # 主窗口实现
├── webbridge.*             # Web-Qt 通信桥
├── struct/                 # 数据结构定义
├── web/                    # Web 前端资源
│   └── vue-proj/           # Vue 项目
├── bin/                    # 编译输出目录
├── resources.qrc           # Qt 资源文件
└── doc/                    # 项目文档
```

## 💡 主要功能

- **学生信息管理**：全面的 CRUD 操作界面
- **数据管理工具**：数据导入导出、备份恢复
- **多视图展示**：表格视图、卡片视图、统计视图
- **设置中心**：用户偏好和系统配置持久化
- **通知系统**：重要事件的系统级通知

## 📊 技术分布

- Vue: 55.5%
- C++: 29.7%
- Go: 5.2%
- CMake: 3.3%
- JavaScript: 2.6%
- Rust: 1.6%
- 其他: 2.1%

## 🤝 参与贡献

欢迎提交 Pull Request 或创建 Issue！在参与项目前，请先熟悉代码库并遵循项目编码风格。

## 📄 许可证

本项目采用 MIT 许可证 - 详情请查看 [LICENSE](LICENSE) 文件

---

© 2025 [Liu Yang (LithiumValproate)](https://github.com/LithiumValproate) - Xinjiang University