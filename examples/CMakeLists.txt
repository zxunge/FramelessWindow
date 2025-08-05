cmake_minimum_required(VERSION 3.22)

project(FramelessWindow LANGUAGES C CXX)

set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

qt_standard_project_setup()

qt_add_executable(FramelessWindowExample
    main.cpp
)

set_target_properties(FramelessWindowExample PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(FramelessWindowExample PRIVATE Qt::Widgets FramelessWindow)
