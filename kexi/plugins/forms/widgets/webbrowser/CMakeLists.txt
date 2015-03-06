include_directories(    ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/plugins/forms/widgets/webbrowser ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/main ${KDE4_INCLUDES}  )

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
    ${KOPROPERTY_LIBS}
    
    ${KDE4_KDEUI_LIBS}
    Qt5::Core
    Qt5::Gui
    ${QT_QTWEBKIT_LIBRARY}
    Qt5::Xml
    )


install(TARGETS kformdesigner_webbrowser  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kformdesigner_webbrowser.desktop DESTINATION
        ${SERVICES_INSTALL_DIR}/calligra)

ecm_install_icons(${DATA_INSTALL_DIR}/kexi/icons)
