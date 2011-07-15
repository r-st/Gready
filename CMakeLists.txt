project(gready)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(gready_SRCS gready.cpp main.cpp reader/reader.cpp)
qt4_automoc(${gready_SRCS})
add_executable(gready ${gready_SRCS})
target_link_libraries(gready ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})
