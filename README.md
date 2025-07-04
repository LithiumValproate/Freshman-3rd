# Qt Web学生管理系统

一个将现代Web学生管理系统无缝集成到Qt桌面应用的项目，兼具Web灵活性与原生桌面体验，适用于跨平台学生信息管理。

## ✨ 项目亮点
- **QWebEngineView** 嵌入Web前端，原生桌面窗口体验
- **Web-Qt双向通信**：QWebChannel实现前后端高效交互
- **数据管理增强**：自动备份、恢复、设置持久化
- **现代UI/UX**：响应式布局、状态栏、进度条、全屏等
- **系统集成**：托盘、快捷键、原生文件对话框、系统通知

## 📦 技术栈
- Qt 5/6 (C++，QWebEngine)
- QWebChannel
- HTML/CSS/JavaScript (前端)
- 可选：Go后端（如有）

## 🚀 快速开始
### 1. 克隆仓库
```bash
git clone <本项目地址>
cd Freshman-3rd
```
### 2. 构建与运行
#### Qt桌面端
1. 使用Qt Creator或命令行编译：
   ```bash
   qmake && make
   ./bin/QtWebStudentSys.app/Contents/MacOS/QtWebStudentSys
   ```
2. 或用CMake：
   ```bash
   cmake .
   make
   ./bin/QtWebStudentSys.app/Contents/MacOS/QtWebStudentSys
   ```
#### Web前端（可选）
```bash
cd web/vue-proj
npm install
npm run dev
```

## 📁 目录结构
- `main.cpp`/`mainwindow.*`：Qt主程序与主窗口
- `webbridge.*`：Web与Qt通信桥
- `web/`：Web前端资源
- `bin/`：编译输出
- `resources.qrc`：Qt资源文件
- `struct/`：数据结构定义

## 🛠️ 主要功能
- 学生信息增删查改
- 数据备份与恢复
- 设置持久化
- Web与桌面端实时通信
- 系统托盘与快捷操作

## 🤝 贡献
欢迎PR和Issue！请先阅读代码，遵循项目风格。

## 📄 许可证
MIT License

---
如需详细开发文档，请查阅`doc/`目录。
