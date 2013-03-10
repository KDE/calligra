project(calligraqtquickcomponents)

set(calligraqtquickcomponents_SRCS
    CalligraQtQuickComponentsPlugin.cpp
    CQCanvasController.cpp
    CQTextDocumentCanvas.cpp
    )

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${KDE4_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${CMAKE_SOURCE_DIR}/words/part
)

qt4_automoc(${calligraqtquickcomponents_SRCS})

add_library(calligraqtquickcomponentsplugin SHARED ${calligraqtquickcomponents_SRCS})
target_link_libraries(calligraqtquickcomponentsplugin
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    ${KDECLARATIVE_LIBRARIES}
    komain
    kordf
    wordsprivate
    calligrasheetscommon
    kopageapp
    calligrastageprivate)

install(TARGETS calligraqtquickcomponentsplugin DESTINATION ${DATA_INSTALL_DIR}/calligra/imports)
install(FILES qmldir DESTINATION ${DATA_INSTALL_DIR}/calligra/imports)