project(calligraqtquickcomponents)

set(calligraqtquickcomponents_SRCS
    CQThumbnailItem.cpp
    CQPresentationModel.cpp
#    CQSpreadsheetListModel.cpp
    CQCanvasBase.cpp
    CQCanvasControllerItem.cpp
#    CQSpreadsheetCanvas.cpp
    CalligraQtQuickComponentsPlugin.cpp
    CQCanvasController.cpp
    CQTextDocumentCanvas.cpp
    CQTextDocumentModel.cpp
    CQImageProvider.cpp
    CQPresentationView.cpp
    CQPresentationCanvas.cpp
    CQTextToCModel.cpp
    CQPluginLoaderProxy.cpp
    CQLinkArea.cpp
    CQTextDocumentNotesModel.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/words/part
    ${CMAKE_SOURCE_DIR}/words/gemini
#    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/libs/kopageapp
)

automoc4_add_library(calligraqtquickcomponentsplugin MODULE ${calligraqtquickcomponents_SRCS})

target_link_libraries(calligraqtquickcomponentsplugin
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_LIBRARIES}
    komain
    kotextlayout
    wordsprivate
#    calligrasheetscommon
    kopageapp
    calligrastageprivate)

install(TARGETS calligraqtquickcomponentsplugin
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/calligra/CalligraComponents)
install(FILES qmldir DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/calligra/CalligraComponents)
