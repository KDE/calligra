project(calligraqtquickcomponents)

set(calligraqtquickcomponents_SRCS
    CQThumbnailItem.cpp
    CQSpreadsheetListModel.cpp
    CQCanvasBase.cpp
    CQCanvasControllerItem.cpp
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

kde4_add_plugin(calligraqtquickcomponentsplugin WITH_PREFIX ${calligraqtquickcomponents_SRCS})
target_link_libraries(calligraqtquickcomponentsplugin
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    wordsprivate
    calligrasheetscommon
    kopageapp
    calligrastageprivate)

install(TARGETS calligraqtquickcomponentsplugin
    DESTINATION ${LIB_INSTALL_DIR}/calligra/imports/org/calligra/CalligraComponents)
install(FILES qmldir DESTINATION ${LIB_INSTALL_DIR}/calligra/imports/org/calligra/CalligraComponents)
