cmake_minimum_required(VERSION 3.16)

project(QtWebSchoolSys VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# ====================================================================
# 【【【决定性的最终修正 - Part 1】】】
# 使用 set_target_properties 强制将 im_core 的输出（.dll 和 .lib）
# 全部放到一个固定的、不带配置子目录的 build/lib 文件夹中。
set_target_properties(im_core PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
# ====================================================================

# 创建可执行文件
add_executable(QtWebSchoolSys
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
)

# 【【【决定性的最终修正 - Part 2】】】
# 强制将 QtWebSchoolSys 的输出（.exe）放到 build/bin 文件夹中。
set_target_properties(QtWebSchoolSys PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 【【【决定性的最终修正 - Part 3】】】
# 明确告诉 QtWebSchoolSys 的链接器：当你需要找 .lib 文件时，
# 请务必到我们上面指定的 build/lib 文件夹里去找！
target_link_directories(QtWebSchoolSys PRIVATE
        "${CMAKE_BINARY_DIR}/lib"
)
# ====================================================================

# 链接Qt库 和 你的 im_core 库
target_link_libraries(QtWebSchoolSys PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::WebEngineWidgets
        Qt6::WebChannel
        Qt6::Sql
        im_core
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

# 拷贝web和icons资源文件到 .exe 所在的目录
set(EXE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/web/vue-proj/dist")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/web/vue-proj/dist/ DESTINATION ${EXE_OUTPUT_PATH}/web)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/icons/ DESTINATION ${EXE_OUTPUT_PATH}/icons)
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
        $<$<CONFIG:Debug>:DEBUG>
)