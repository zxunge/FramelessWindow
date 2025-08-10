target_sources(FramelessWindow PRIVATE
    winframelessmainwindow.cpp
	winframelessmainwindow.h
	winframelessmainwindow.ui
)
target_include_directories(FramelessWindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(FramelessWindow PRIVATE dwmapi)