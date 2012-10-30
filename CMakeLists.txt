project(gready)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 COMPONENTS QtWebKit REQUIRED)
find_package(QJSON COMPONENTS qjson REQUIRED )

set(EXTERNAL_LIBS ${EXTERNAL_LIBS} ${QJSON_LIBRARIES})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${QJSON_INCLUDE_DIR})

set(gready_SRCS gui/settingswindow.cpp reader/article.cpp reader/feed.cpp  reader/reader.cpp reader/tag.cpp gui/mainwindow.cpp gready.cpp main.cpp)
qt4_automoc(${gready_SRCS})
add_executable(gready ${gready_SRCS})
target_link_libraries(gready ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${QJSON_LIBRARIES})
