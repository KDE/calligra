cmake_minimum_required(VERSION 2.6)
project(calligra-active)

find_package(Qt4 4.7.0 REQUIRED)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QT_INCLUDES}
                    ${KDE4_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/words/part
    ${CMAKE_SOURCE_DIR}/tables
    ${CMAKE_SOURCE_DIR}/libs/kopageapp
)

set (calligra-active_RCCS CalligraActive.qrc)
qt4_add_resources(calligra-active_RCC_SRCS ${calligra-active_RCCS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/calligra_active_global.h.in ${CMAKE_CURRENT_BINARY_DIR}/calligra_active_global.h)

set(calligra-active_SRCS
    src/CAPresentationHandler.cpp
    src/CASpreadsheetHandler.cpp
    src/CATextDocumentHandler.cpp
    src/CAAbstractDocumentHandler.cpp
    src/CADocumentController.cpp
    src/CADocumentInfo.cpp
    src/PAView.cpp
    src/CanvasController.cpp
    src/MainWindow.cpp
    main.cpp
    ${calligra-active_RCC_SRCS}
)

qt4_automoc(${calligra-active_SRCS})

add_executable(calligraactive ${calligra-active_SRCS})

target_link_libraries(calligraactive
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    wordsprivate
    calligratablescommon
    kopageapp
)

install(TARGETS calligraactive RUNTIME DESTINATION bin)
install(FILES calligraactive.desktop DESTINATION share/applications)
install(DIRECTORY qml DESTINATION share/calligraactive)
install(DIRECTORY templates DESTINATION share/calligraactive)
