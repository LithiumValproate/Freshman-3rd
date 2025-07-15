cmake_minimum_required(VERSION 3.16)

project(QtWebSchoolSys VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ====================================================================
# 【【【核心修正】】】
# 为所有目标设置统一的、按配置分类的输出目录
# 这会创建 build/bin/Release, build/lib/Release 等标准结构
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
# ====================================================================

# 查找Qt组件
find_package(Qt6 REQUIRED COMPONENTS
        Core
        Widgets
        WebEngineWidgets
        WebChannel
        Sql
)

# 启用Qt的MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置源文件、头文件、资源文件
set(SOURCES
        main.cpp
        mainwindow.cpp
        webbridge.cpp
)
set(HEADERS
        mainwindow.h
        webbridge.h
        struct/student.h
        struct/stu_with_score.h
        struct/other_users.h
        struct/course.h
        im/message.h
        im/user.h
        im/room.h
)
set(RESOURCES
        resources.qrc
)

# 创建 im_core 共享库
add_library(im_core SHARED
        im/user.cpp
        im/room.cpp
        im/im_go_bridge/im_bridge.cpp
)

# 为 im_core 设置头文件包含目录
target_include_directories(im_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/struct
        ${CMAKE_CURRENT_SOURCE_DIR}/im/im_go_bridge
)

# 【【【修正】】】 不再需要为单个目标设置输出目录，已由全局设置取代
# set_target_properties(im_core ...)

# 创建可执行文件
add_executable(QtWebSchoolSys
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
)

# 链接Qt库 和 你的 im_core 库
target_link_libraries(QtWebSchoolSys PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::WebEngineWidgets
        Qt6::WebChannel
        Qt6::Sql
        im_core  # CMake 会自动处理 Debug/Release 的链接
)

# 为主程序设置头文件包含目录
target_include_directories(QtWebSchoolSys PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# macOS特定设置
if(APPLE)
    set_target_properties(QtWebSchoolSys PROPERTIES
            MACOSX_BUNDLE TRUE
    )
endif()

# 安装设置
install(TARGETS QtWebSchoolSys
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
)

# 拷贝web和icons资源文件到构建目录
# ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} 现在会自动指向 build/bin/Release
message(STATUS "Looking for Vue build output in ${CMAKE_CURRENT_SOURCE_DIR}/web/vue-proj/dist")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/web/vue-proj/dist")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/web/vue-proj/dist/
            DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/web) # 复制到 exe 旁边
    message(STATUS "Copied Vue build output to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/web")
else()
    message(WARNING "Vue build output not found. Please run 'pnpm build' in 'web/vue-proj' first.")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/icons/
            DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/icons) # 复制到 exe 旁边
    message(STATUS "Copied icons to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/icons")
else()
    message(WARNING "Icons folder not found in project root.")
endif()

# 编译选项和定义
if(MSVC)
    target_compile_options(QtWebSchoolSys PRIVATE /W4)
else()
    target_compile_options(QtWebSchoolSys PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_definitions(QtWebSchoolSys PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
        USE_QTJSON
        # DEBUG 宏只应在 Debug 模式下定义
)

# 为 Debug 配置单独添加定义
target_compile_definitions(QtWebSchoolSys PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)