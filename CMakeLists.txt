cmake_minimum_required(VERSION 3.22)

project(FramelessWindow LANGUAGES CXX)

set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_EXAMPLES "Build the examples." OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

qt_standard_project_setup()

qt_add_library(FramelessWindow STATIC
    framelessmainwindow.cpp
    framelessmainwindow.h
    framelessmainwindow.ui
    framelessdialog.cpp
    framelessdialog.h
    framelessdialog.ui
    flesshelper.h
    fwres.qrc
)

if(WIN32)
    add_subdirectory(win)
endif()

target_link_libraries(FramelessWindow PRIVATE Qt::Core Qt::Gui Qt::Widgets Qt::Svg)

target_compile_definitions(FramelessWindow PRIVATE FRAMELESSWINDOW_LIBRARY)

target_include_directories(FramelessWindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
