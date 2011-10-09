include_directories(    ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/plugins/forms/widgets/webbrowser ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/main ${KDE4_INCLUDES}  )

set(kformdesigner_webbrowser_PART_SRCS 
    WebBrowserWidget.cpp
    WebBrowserFactory.cpp)


kde4_add_plugin(kformdesigner_webbrowser  ${kformdesigner_webbrowser_PART_SRCS})

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
    ${KOPROPERTY_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTWEBKIT_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY} 
    )


install(TARGETS kformdesigner_webbrowser  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kformdesigner_webbrowser.desktop DESTINATION
        ${SERVICES_INSTALL_DIR}/kformdesigner)

kde4_install_icons(${DATA_INSTALL_DIR}/kexi/icons)
