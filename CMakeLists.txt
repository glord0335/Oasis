cmake_minimum_required(VERSION 3.10)
project(Oasis VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动处理 Qt 的 MOC, RCC 和 UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Svg)

# 源文件列表
set(SOURCES
    src/main.cpp
    src/ui/popup_widget.cpp
    src/core/reminder_engine.cpp
    src/core/plant_system.cpp
    src/core/settings_manager.cpp
    src/ui/components/circular_progress.cpp
    src/ui/stats_widget.cpp
    src/ui/settings_widget.cpp
)

# 资源文件
set(RESOURCES
    resources/resources.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::Gui
    Qt5::Svg
)

# 安装规则 (可选)
# install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
