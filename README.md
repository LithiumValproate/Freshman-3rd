# UnivUni_Platform: A Hybrid Desktop & Web Student System

<div align="center">

![Languages](https://img.shields.io/badge/Languages-C++%20%7C%20Vue%20%7C%20Go%20%7C%20Python-blue)
![Frameworks](https://img.shields.io/badge/Frameworks-Qt%20%7C%20Vue.js%20%7C%20Vite-green)

</div>

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

___

<div align="center">

© 2025 [LithiumValproate](https://github.com/LithiumValproate)

</div>
