include_directories(
    ${CMAKE_SOURCE_DIR}/kexi/formeditor
    ${CMAKE_SOURCE_DIR}/kexi/core
)

set(kexiforms_webbrowserwidgetplugin_SRCS
    WebBrowserWidget.cpp
    WebBrowserFactory.cpp
)

add_library(kexiforms_webbrowserwidgetplugin MODULE ${kexiforms_webbrowserwidgetplugin_SRCS})
kcoreaddons_desktop_to_json(kexiforms_webbrowserwidgetplugin kexiforms_webbrowserwidgetplugin.desktop)

set (QT_USE_QTWEBKIT TRUE)

target_link_libraries(kexiforms_webbrowserwidgetplugin
    kformdesigner
    kexicore
    kexiguiutils
    kexidatatable
    kexiextendedwidgets
    kexidataviewcommon
    kexiformutils

    Qt5::Core
    Qt5::Gui
    Qt5::WebKitWidgets
    Qt5::Xml
)

install(TARGETS kexiforms_webbrowserwidgetplugin DESTINATION ${KEXI_FORM_WIDGETS_PLUGIN_INSTALL_DIR})

ecm_install_icons(ICONS
    16-actions-web_browser.png
    22-actions-web_browser.png
    DESTINATION ${DATA_INSTALL_DIR}/kexi/icons
    THEME hicolor
)
