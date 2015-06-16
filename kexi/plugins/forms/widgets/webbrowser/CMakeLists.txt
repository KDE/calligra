include_directories(    ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/plugins/forms/widgets/webbrowser ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/main   )

set(kformdesigner_webbrowser_PART_SRCS
    WebBrowserWidget.cpp
    WebBrowserFactory.cpp)


add_library(kformdesigner_webbrowser MODULE  ${kformdesigner_webbrowser_PART_SRCS})

set (QT_USE_QTWEBKIT TRUE)

target_link_libraries(
    kformdesigner_webbrowser
    kformdesigner
    kexicore
    kexiguiutils
    kexidatatable
    kexiextendedwidgets
    kformdesigner
    kexidataviewcommon
    kexiformutils

    KProperty

    Qt5::Core
    Qt5::Gui
    Qt5::WebKitWidgets
    Qt5::Xml
)


install(TARGETS kformdesigner_webbrowser  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})

########### install files ###############

ecm_install_icons(ICONS
    16-actions-web_browser.png
    22-actions-web_browser.png
    DESTINATION ${DATA_INSTALL_DIR}/kexi/icons
    THEME hicolor
)
