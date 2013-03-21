project(calligraqtquickcomponents)

set(calligraqtquickcomponents_SRCS
    CQSpreadsheetCanvas.cpp
    CalligraQtQuickComponentsPlugin.cpp
    CQCanvasController.cpp
    CQTextDocumentCanvas.cpp
    CQTextDocumentModel.cpp
    CQImageProvider.cpp
    CQPresentationView.cpp
    CQPresentationCanvas.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/words/part
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/libs/kopageapp
)

qt4_automoc(${calligraqtquickcomponents_SRCS})

add_library(calligraqtquickcomponentsplugin SHARED ${calligraqtquickcomponents_SRCS})
target_link_libraries(calligraqtquickcomponentsplugin
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    wordsprivate
    calligrasheetscommon
    kopageapp
    calligrastageprivate)

install(TARGETS calligraqtquickcomponentsplugin
    DESTINATION ${DATA_INSTALL_DIR}/calligra/imports/org/calligra/CalligraComponents)
install(FILES qmldir DESTINATION ${DATA_INSTALL_DIR}/calligra/imports/org/calligra/CalligraComponents)
