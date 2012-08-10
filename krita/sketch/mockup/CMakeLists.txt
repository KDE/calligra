cmake_minimum_required(VERSION 2.6)

project(krita-sketch)

find_package(Qt 4.8 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(KDE4Internal REQUIRED)
find_package(Nepomuk REQUIRED)
find_package(CalligraLibs 2.4 REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDES} ${NEPOMUK_INCLUDE_DIRS} ${QT_INCLUDES} ${CALLIGRALIBS_INCLUDE_DIR})

# Krita includes etc
include_directories(../../)
FIND_LIBRARY(KRITAUI_LIBRARY NAMES kritaui)

set(krita-sketch_SRCS
    constants.cpp
    settings.cpp
    main.cpp
    mainwindow.cpp
    presetimageprovider.cpp
    presetmodel.cpp
)

qt4_automoc(${krita-sketch_SRCS})
qt4_add_resources(krita-sketch_SRCS krita-sketch.qrc)

add_executable(krita-sketch ${krita-sketch_SRCS})
target_link_libraries(krita-sketch ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${KOMAIN_LIBRARY} ${KOWIDGETS_LIBRARY} ${PIGMENTCMS_LIBRARY} ${KRITAUI_LIBRARY})

add_custom_target(QmlFiles ALL ${CMAKE_COMMAND} -E echo "Main application QML files"
    SOURCES
    MainPage.qml
    SettingsPage.qml
    SharePage.qml
    WelcomePage.qml
)
add_custom_target(QmlComponentFiles ALL ${CMAKE_COMMAND} -E echo "QML Component files"
    SOURCES
    components/Button.qml
    components/CategorySwitcher.qml
    components/Divider.qml
    components/DropShadow.qml
    components/Header.qml
    components/Label.qml
    components/NewImageList.qml
    components/NewsList.qml
    components/Page.qml
    components/PageStack.js
    components/PageStack.qml
    components/RecentFilesList.qml
    components/TextField.qml
)
add_custom_target(QmlPanelFiles ALL ${CMAKE_COMMAND} -E echo "Panel QML files"
    SOURCES
    panels/ColorPanel.qml
    panels/EditPresetPage.qml
    panels/FilterPanel.qml
    panels/LayersPanel.qml
    panels/MenuPanel.qml
    panels/NewImagePanel.qml
    panels/OpenImagePanel.qml
    panels/PanelBar.qml
    panels/Panel.qml
    panels/PresetsPanel.qml
    panels/SelectPanel.qml
    panels/ToolPanel.qml
)
