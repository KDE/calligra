include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/main ${KDE4_INCLUDES}  )

include_directories(${MARBLE_INCLUDE_DIR})


set(kformdesigner_mapbrowser_PART_SRCS 
    MapBrowserWidget.cpp
    MapBrowserFactory.cpp)


kde4_add_plugin(kformdesigner_mapbrowser  ${kformdesigner_mapbrowser_PART_SRCS})



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
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    )

install(TARGETS kformdesigner_mapbrowser  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kformdesigner_mapbrowser.desktop DESTINATION
        ${SERVICES_INSTALL_DIR}/calligra)

kde4_install_icons(${DATA_INSTALL_DIR}/kexi/icons)
