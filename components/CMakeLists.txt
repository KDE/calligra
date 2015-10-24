project(calligracomponentsplugin)

set(components_SRCS
    CalligraComponentsPlugin.cpp
    Document.cpp
    Enums.cpp
    Global.cpp
    ImageDataItem.cpp
    LinkArea.cpp
    View.cpp
    ViewController.cpp

    models/ContentsModel.cpp
    models/PresentationContentsModelImpl.cpp
    models/SpreadsheetContentsModelImpl.cpp
    models/TextContentsModelImpl.cpp

    impl/DocumentImpl.cpp
    impl/TextDocumentImpl.cpp
    impl/SpreadsheetImpl.cpp
    impl/PresentationImpl.cpp
    impl/PresentationKoPAView.cpp
    impl/ComponentsKoCanvasController.cpp
)

add_library(CalligraComponentsPlugin SHARED ${components_SRCS})
target_include_directories(CalligraComponentsPlugin
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries( CalligraComponentsPlugin
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    wordsprivate
    calligrasheetscommon
    calligrastageprivate
    kotextlayout
)

install(TARGETS CalligraComponentsPlugin DESTINATION ${QML_INSTALL_DIR}/org/kde/calligra)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/calligra)
