cmake_minimum_required(VERSION 2.6)

project(krita-sketch)

find_package(Qt 4.8 REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

set(krita-sketch_SRCS
    constants.cpp
    main.cpp
)

qt4_automoc(${krita-sketch_SRCS})
qt4_add_resources(krita-sketch_SRCS krita-sketch.qrc)

add_executable(krita-sketch ${krita-sketch_SRCS})
target_link_libraries(krita-sketch ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY})
