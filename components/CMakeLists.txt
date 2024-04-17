project(calligracomponentsplugin)

# TEMPORARY: for  Qt6/KF6 build porting phase deprecation warnings are only annoying noise
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-deprecated -Wno-deprecated-declarations)
endif ()

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

if(WIN32)
add_library(CalligraComponentsPlugin MODULE ${components_SRCS})
else()
add_library(CalligraComponentsPlugin SHARED ${components_SRCS})
endif()
target_include_directories(CalligraComponentsPlugin
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries( CalligraComponentsPlugin
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    wordsprivate
    calligrasheetspartlib
    calligrastageprivate
    kotextlayout
    koplugin
)

install(TARGETS CalligraComponentsPlugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/calligra)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/calligra)
