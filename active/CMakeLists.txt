cmake_minimum_required(VERSION 2.6)
project(calligra-mobile)

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

set (calligra-mobile_RCCS CalligraMobile.qrc)
qt4_add_resources(calligra-mobile_RCC_SRCS ${calligra-mobile_RCCS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/calligra_mobile_global.h.in ${CMAKE_CURRENT_BINARY_DIR}/calligra_mobile_global.h)

set(calligra-mobile_SRCS
    src/CADocumentInfo.cpp
    src/PAView.cpp
    src/CanvasController.cpp
    src/MainWindow.cpp
    main.cpp
    ${calligra-mobile_RCC_SRCS}
)

qt4_automoc(${calligra-mobile_SRCS})

add_executable(calligra-mobile ${calligra-mobile_SRCS})

target_link_libraries(calligra-mobile
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    wordsprivate
    calligratablescommon
    kopageapp
)

install(TARGETS calligra-mobile RUNTIME DESTINATION bin)
install(DIRECTORY qml DESTINATION share/calligra-mobile)
install(DIRECTORY templates DESTINATION share/calligra-mobile)
