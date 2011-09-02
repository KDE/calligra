include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/main ${KDE4_INCLUDES}  )

find_package(Marble REQUIRED)

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
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )
#kexicore
#kexiguiutils
#kexidatatable
#kexiextendedwidgets
#kformdesigner
#${KOPROPERTY_LIBS}
#kexiformutils

install(TARGETS kformdesigner_mapbrowser  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kformdesigner_mapbrowser.desktop DESTINATION
        ${SERVICES_INSTALL_DIR}/kformdesigner)

