include_directories(
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/formeditor
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/libs/main
    ${MARBLE_INCLUDE_DIR}
    
)


set(kformdesigner_mapbrowser_PART_SRCS 
    MapBrowserWidget.cpp
    MapBrowserFactory.cpp)


add_library(kformdesigner_mapbrowser MODULE  ${kformdesigner_mapbrowser_PART_SRCS})



target_link_libraries(
    kformdesigner_mapbrowser
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

install(TARGETS kformdesigner_mapbrowser  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kformdesigner_mapbrowser.desktop DESTINATION
        ${SERVICES_INSTALL_DIR}/calligra)

ecm_install_icons(ICONS
    16-actions-map_browser.png
    22-actions-map_browser.png
    DESTINATION ${DATA_INSTALL_DIR}/kexi/icons
    THEME hicolor
)
