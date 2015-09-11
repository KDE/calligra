include_directories(
    ${CMAKE_SOURCE_DIR}/kexi/formeditor
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${MARBLE_INCLUDE_DIR}
)

set(kexiforms_mapwidgetplugin_SRCS
    MapBrowserWidget.cpp
    MapBrowserFactory.cpp)

add_library(kexiforms_mapwidgetplugin MODULE ${kexiforms_mapwidgetplugin_SRCS})
kcoreaddons_desktop_to_json(kexiforms_mapwidgetplugin kexiforms_mapwidgetplugin.desktop)

target_link_libraries(kexiforms_mapwidgetplugin
    kformdesigner
    kexicore
    kexiguiutils
    kexidatatable
    kexiextendedwidgets
    kexiformutils
    kexidataviewcommon
    ${MARBLE_LIBRARIES}

    Qt5::Core
    Qt5::Gui
    Qt5::Xml
)

install(TARGETS kexiforms_mapwidgetplugin DESTINATION ${KEXI_FORM_WIDGETS_PLUGIN_INSTALL_DIR})

ecm_install_icons(ICONS
    16-actions-map_browser.png
    22-actions-map_browser.png
    DESTINATION ${DATA_INSTALL_DIR}/kexi/icons
    THEME hicolor
)
