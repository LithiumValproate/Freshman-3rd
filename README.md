# UnivUni_Platform: A Hybrid Desktop & Web Student System

<div align="center">

![Languages](https://img.shields.io/badge/Languages-C++%20%7C%20Vue%20%7C%20Go%20%7C%20Python-blue)
![Frameworks](https://img.shields.io/badge/Frameworks-Qt%20%7C%20Vue.js%20%7C%20Vite-green)
![License](https://img.shields.io/badge/License-MIT-yellow)
![Stars](https://img.shields.io/github/stars/LithiumValproate/Freshman-3rd?style=social)

*A modern, integrated platform for student management and real-time communication, built with Qt and Web technologies.*

</div>

## 📝 Project Overview

**UnivUni_Platform** is an innovative, hybrid application that combines a powerful C++ Qt backend with a modern Vue.js frontend. It provides a comprehensive student management system and features a fully integrated, real-time Instant Messaging (IM) service built with Go and C++.

The frontend is a responsive Vue 3 application, rendered within a native Qt desktop window using `QWebEngineView`. This architecture leverages the flexibility of web development for the UI and the power of C++/Go for high-performance backend logic, offering a seamless and feature-rich cross-platform experience.

## ✨ Core Features

- **🎨 Modern Web UI**: A beautiful and responsive interface built with **Vue 3** and **Vite**.
- **🔗 Hybrid Architecture**: Seamlessly embeds the web frontend into a Qt desktop application for a native feel.
- **⚡ Real-time Communication**: Features a C++/Go powered Instant Messaging module for live chat.
- **🔄 Bidirectional Bridge**: Uses `QWebChannel` to enable efficient, real-time data exchange between the C++ backend and the Vue frontend.
- **🛡️ Crash Reporting**: Integrated with **Sentry** for robust, real-time crash and error monitoring.
- **🔧 Multi-Language Backend**: Utilizes C++ for core application logic, Go for networking services, and Python for data utility scripts.
- **📱 Responsive Design**: The UI is fully responsive and adapts to various screen and window sizes.

## 🔧 Technology Stack

### Frontend
- **Vue 3**: Core frontend framework.
- **Vite**: Next-generation frontend tooling for fast development.
- **Vue Router**: For client-side routing.
- **JavaScript & CSS**: For application logic and styling.

### Backend & Desktop
- **C++ (Qt 6)**: Main application framework, business logic, and desktop integration.
- **Go**: Powers the Instant Messaging (IM) client and backend services.
- **Python**: Used for utility scripts, such as JSON/SQL data conversion.

### Core Libraries & Tools
- **QWebEngine**: Chromium-based engine for rendering the web UI.
- **QWebChannel**: The bridge for C++ and JavaScript communication.
- **Sentry Native**: For native crash reporting.
- **CMake & QMake**: For building the C++ application.

## 🚀 Getting Started

### 📋 Prerequisites

- **Qt Development Environment**: Qt 6.x or newer.
- **Compiler**: A C++17 compatible compiler (MSVC, GCC, Clang).
- **Node.js**: Version 16+ with npm or yarn.
- **Go**: Version 1.19+ for the IM module.
- **CMake**: Version 3.16+ for the build system.
- **Python**: Version 3.8+ for data scripts.

### 📥 Installation

1.  Clone the repository:
    ```bash
    git clone https://github.com/LithiumValproate/Freshman-3rd.git
    cd Freshman-3rd
    ```

### 🔨 Build & Run

#### 1. Build the Vue.js Frontend

The frontend must be built first as its output is bundled into the Qt application resources.

```bash
cd web/vue-proj
npm install
npm run build
```

#### 2. Build the Qt Application

We recommend using CMake for the build process.

```bash
# Create and navigate to the build directory
mkdir build && cd build

# Configure the project
cmake ..

# Build the application
cmake --build .

# Run the application (executable path may vary by OS)
./bin/QtWebSchoolSys
```

## 📂 Project Structure

```
UnivUni_Platform/
├── 📁 im/                   # C++ Instant Messaging backend logic
├── 📁 Go/                   # Go module for IM services
├── 📁 web/vue-proj/         # Vue 3 + Vite frontend project
│   ├── src/                # Vue source code (views, components, router)
│   └── dist/               # Built frontend assets (bundled by Qt)
├── 📁 struct/               # C++ data structure definitions (students, courses, etc.)
├── 📁 sql_link/             # Python scripts for data conversion
├── 📁 sentry-native/        # Sentry SDK for crash reporting
├── 📄 main.cpp              # Qt application entry point
├── 📄 mainwindow.h/.cpp     # Main desktop window implementation
├── 📄 webbridge.h/.cpp      # QWebChannel communication bridge
├── 📄 CMakeLists.txt        # Primary CMake build configuration
└── 📄 resources.qrc         # Qt resource file (bundles web assets)
```

## 💡 Key Functionality

- **Student Management**: Full CRUD (Create, Read, Update, Delete) operations for student records.
- **Instant Messaging**: Real-time chat functionality between users (students, teachers, admins).
- **Role-Based Access Control**: Different views and permissions for Students, Teachers, and Administrators.
- **Data Persistence**: User and session data is managed via local storage.
- **System Integration**: Leverages native desktop features like system notifications and dialogs.

## 🤝 How to Contribute

Contributions are welcome! If you'd like to help improve the project, please follow these steps:

1.  **Fork** the repository.
2.  Create a new branch for your feature (`git checkout -b feature/YourAmazingFeature`).
3.  Commit your changes (`git commit -m 'Add YourAmazingFeature'`).
4.  Push to your branch (`git push origin feature/YourAmazingFeature`).
5.  Open a **Pull Request**.

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## 📞 Contact

- **Author**: Liu Yang (LithiumValproate)
- **Institution**: Xinjiang University

---

<div align="center">

**⭐ If you find this project useful, please consider giving it a star! ⭐**

© 2025 [LithiumValproate](https://github.com/LithiumValproate)

</div>
