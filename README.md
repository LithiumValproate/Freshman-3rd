# Qt-Web 学生管理系统

<div align="center">

![Language](https://img.shields.io/badge/Languages-Vue%20|%20C++%20|%20Go%20|%20JavaScript%20|%20Python-blue)
![Framework](https://img.shields.io/badge/Framework-Qt%20|%20Vue.js%20|%20Vite-green)
![License](https://img.shields.io/badge/License-MIT-yellow)
![Stars](https://img.shields.io/github/stars/LithiumValproate/Freshman-3rd?style=social)

*一个现代化的桌面-Web融合学生管理系统*

</div>

## 📝 项目概述

Qt-Web 学生管理系统是一个创新性融合项目，将现代 Web 前端技术与 Qt 桌面应用完美结合。系统采用 Vue 3 + Vite 构建前端界面，通过 QWebEngineView 嵌入到 Qt 桌面应用中，为学生信息管理提供了既具备 Web 应用灵活性又拥有桌面应用性能的跨平台解决方案。

## ✨ 核心特性

- **🎨 现代化前端**: Vue 3 + Vite 构建，支持热重载和快速开发
- **🔗 桌面 Web 融合**: QWebEngineView 嵌入 Vue 前端，原生桌面体验
- **⚡ 双向通信**: 基于 QWebChannel 实现前后端高效实时数据交互
- **🛡️ 数据安全**: 自动备份恢复机制，设置持久化存储
- **📱 响应式设计**: 适配不同屏幕尺寸，现代化 UI/UX 体验
- **🔧 系统集成**: 系统托盘、全局快捷键、原生对话框、系统通知

## 🔧 技术栈

### 前端技术栈
- **Vue 3** (56.8%): 现代响应式前端框架
- **Vite**: 快速构建工具和开发服务器
- **JavaScript** (2.0%): 客户端逻辑处理
- **CSS** (0.6%): 样式设计
- **HTML** (0.2%): 页面结构

### 后端技术栈
- **C++** (31.4%): Qt 框架核心应用逻辑
- **Go** (4.1%): 辅助服务和数据处理模块
- **Python** (0.5%): 脚本工具和数据分析
- **CMake** (3.9%): 跨平台构建系统
- **QMake** (0.3%): Qt 项目构建工具

### 核心框架
- **Qt 5/6**: 跨平台桌面应用程序框架
- **QWebEngine**: 现代 Web 内容渲染引擎
- **QWebChannel**: JavaScript 与 Qt 双向通信桥接

## 🚀 快速开始

### 📋 环境要求

**必需环境:**
- Qt 5.15+ 或 Qt 6.x 开发环境
- C++17 或更高版本编译器
- Node.js 16+ 和 npm/yarn
- CMake 3.16+

**可选环境:**
- Go 1.19+ (用于辅助模块)
- Python 3.8+ (用于数据处理脚本)

### 📥 获取代码

```bash
git clone https://github.com/LithiumValproate/Freshman-3rd.git
cd Freshman-3rd
```

### 🔨 构建与运行

#### 1. 构建前端项目
```bash
cd web/vue-proj
npm install
npm run build
```

#### 2. 构建 Qt 桌面应用

**使用 CMake (推荐):**
```bash
mkdir build && cd build
cmake ..
cmake --build .
./bin/QtWebStudentSys
```

**使用 qmake:**
```bash
qmake
make
./bin/QtWebStudentSys
```

#### 3. 开发模式运行

**前端热重载开发:**
```bash
cd web/vue-proj
npm run dev
```

**Qt 应用开发:**
使用 Qt Creator 打开 `.pro` 文件进行开发

## 📂 项目结构

```
Freshman-3rd/
├── 📁 src/                    # Qt 应用源码
│   ├── main.cpp              # 程序入口点
│   ├── mainwindow.*          # 主窗口实现
│   ├── webbridge.*           # Web-Qt 通信桥接
│   ├── struct/               # 数据结构定义
|   ├── im/                   # C++即时通讯后端
|   └── Go/                   # Go 辅助模块  
├── 📁 web/                   # Web 前端项目
│   └── vue-proj/            # Vue 3 + Vite 项目
│       ├── src/             # Vue 源码
│       ├── public/          # 静态资源
│       ├── package.json     # 依赖管理
│       └── vite.config.js   # Vite 配置
├── 📁 bin/                   # 编译输出目录
├── 📁 doc/                   # 项目文档
├── 📄 CMakeLists.txt         # CMake 构建配置
├── 📄 *.pro                  # QMake 项目文件
└── 📄 resources.qrc          # Qt 资源文件
```

## 💡 主要功能

### 🎯 核心功能模块
- **👥 学生信息管理**: 完整的增删改查操作界面
- **📊 数据可视化**: 多维度统计图表和数据分析
- **📁 数据管理**: 导入导出、备份恢复、批量操作
- **🔍 搜索过滤**: 多条件组合搜索和高级筛选
- **⚙️ 设置中心**: 用户偏好和系统配置管理

### 🎨 界面特性
- **📱 响应式布局**: 适配不同窗口尺寸
- **🌙 主题切换**: 支持明暗主题模式
- **🔔 通知系统**: 系统级操作提醒
- **⌨️ 快捷键**: 提升操作效率
- **💾 状态保存**: 自动保存用户操作状态

## 📊 技术组成分析

```
Vue:        114,156 bytes (56.8%) ████████████████████████████████████████
C++:         63,213 bytes (31.4%) ████████████████████████
Go:           8,149 bytes (4.1%)  ███
CMake:        7,778 bytes (3.9%)  ███
JavaScript:   4,077 bytes (2.0%)  █
CSS:          1,269 bytes (0.6%)  
Python:       1,097 bytes (0.5%)  
QMake:          516 bytes (0.3%)  
HTML:           465 bytes (0.2%)  
```

## 🤝 参与贡献

我们欢迎各种形式的贡献！请遵循以下步骤：

1. **Fork** 本仓库
2. 创建您的特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交您的更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开一个 **Pull Request**

### 🔧 开发指南
- 遵循项目现有的代码风格
- 添加适当的注释和文档
- 确保新功能包含相应的测试
- 更新相关文档

## 📋 待办事项

- [ ] 添加单元测试覆盖
- [ ] 完善用户手册文档
- [ ] 支持多语言国际化
- [ ] 添加数据库连接功能
- [ ] 实现插件系统架构

## 📄 许可证

本项目采用 MIT 许可证 - 详情请查看 [LICENSE](LICENSE) 文件

## 📞 联系方式

- **作者**: [Liu Yang (LithiumValproate)](https://github.com/LithiumValproate)
- **邮箱**: [通过 GitHub 联系]
- **学校**: 新疆大学

---

<div align="center">

**⭐ 如果这个项目对您有帮助，请考虑给它一个 Star！**

© 2025 [LithiumValproate](https://github.com/LithiumValproate) - Xinjiang University

</div>