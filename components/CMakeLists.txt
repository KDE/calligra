find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
)

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

kde4_add_library(CalligraComponentsPlugin SHARED ${components_SRCS})
target_link_libraries( CalligraComponentsPlugin
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    wordsprivate
    calligrasheetscommon
    calligrastageprivate
    kotextlayout
)

install(TARGETS CalligraComponentsPlugin DESTINATION ${LIB_INSTALL_DIR}/calligra/org/kde/calligra)
install(FILES qmldir DESTINATION ${LIB_INSTALL_DIR}/calligra/org/kde/calligra)
