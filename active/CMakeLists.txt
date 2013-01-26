cmake_minimum_required(VERSION 2.8.0)

#cmake_policy CMP0017 was introduced in version 2.8.4
if(${CMAKE_VERSION} VERSION_GREATER 2.8.3)
    cmake_policy(SET CMP0017 NEW)
endif()

project(calligra-active)

find_package(Qt4 4.7.0 REQUIRED)
find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QT_INCLUDES}
                    ${KDE4_INCLUDES}
                    ${KDECLARATIVE_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/words/part
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/libs/kopageapp
)

set (calligra-active_RCCS CalligraActive.qrc)
qt4_add_resources(calligra-active_RCC_SRCS ${calligra-active_RCCS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/calligra_active_global.h.in ${CMAKE_CURRENT_BINARY_DIR}/calligra_active_global.h)

set(calligra-active_SRCS
    src/CACanvasItem.cpp
    src/CAImageProvider.cpp
    src/CAPresentationHandler.cpp
    src/CASpreadsheetHandler.cpp
    src/CATextDocumentHandler.cpp
    src/CAAbstractDocumentHandler.cpp
    src/CADocumentController.cpp
    src/CADocumentInfo.cpp
    src/CAPADocumentModel.cpp
    src/CATextDocumentModel.cpp
    src/CAPAView.cpp
    src/CACanvasController.cpp
    src/MainWindow.cpp
    main.cpp
    ${calligra-active_RCC_SRCS}
)

qt4_automoc(${calligra-active_SRCS})

add_executable(calligraactive ${calligra-active_SRCS})

target_link_libraries(calligraactive
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    ${KDECLARATIVE_LIBRARIES}
    komain
    wordsprivate
    calligrasheetscommon
    kopageapp
    calligrastageprivate
)

install(TARGETS calligraactive RUNTIME DESTINATION bin)
install(FILES calligraactive.desktop DESTINATION share/applications)
install(DIRECTORY qml DESTINATION share/calligraactive)
