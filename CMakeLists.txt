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
)

# 启用Qt的MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置源文件
set(SOURCES
    main.cpp
    mainwindow.cpp
    webbridge.cpp
)

# 设置头文件
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

# 设置资源文件
set(RESOURCES
    resources.qrc
)

# Create the IM shared library
add_library(im_core SHARED
    im/user.cpp
    im/room.cpp
    im/im_go_bridge/im_bridge.cpp
    im/message.h
    im/user.h
    im/room.h
)

# Set include directories for im_core
target_include_directories(im_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/struct
    ${CMAKE_CURRENT_SOURCE_DIR}/im/im_go_bridge
)

# Set output directory for im_core
set_target_properties(im_core PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
)

# 创建可执行文件
add_executable(QtWebSchoolSys
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(QtWebSchoolSys PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebChannel
    im_core # <--- ADD THIS
)

# 设置包含目录
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

# 拷贝web资源文件到构建目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/web
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/icons
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 设置输出目录
set_target_properties(QtWebSchoolSys PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
)

# 调试模式设置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(QtWebSchoolSys PRIVATE DEBUG)
endif()

# 编译选项
if(MSVC)
    target_compile_options(QtWebSchoolSys PRIVATE /W4)
else()
    target_compile_options(QtWebSchoolSys PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 设置版本信息
target_compile_definitions(QtWebSchoolSys PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
)
