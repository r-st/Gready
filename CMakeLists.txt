project(gready)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(gready_SRCS reader/article.cpp gui/mainwindow.ui gui/ui_mainwindow.h reader/feed.cpp  reader/reader.cpp reader/tag.cpp gui/mainwindow.cpp gready.cpp main.cpp)
qt4_automoc(${gready_SRCS})
add_executable(gready ${gready_SRCS})
target_link_libraries(gready ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} )
