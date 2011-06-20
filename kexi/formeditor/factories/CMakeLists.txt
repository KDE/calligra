include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/main ${KDE4_INCLUDES}  )

add_definitions(-DKEXI_FORMS_NO_LIST_WIDGET)


########### next target ###############

set(kformdesigner_containers_PART_SRCS containerfactory.cpp )


kde4_add_plugin(kformdesigner_containers ${kformdesigner_containers_PART_SRCS})



target_link_libraries(
    kformdesigner_containers
    kformdesigner
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kformdesigner_containers  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kformdesigner_stdwidgets_PART_SRCS stdwidgetfactory.cpp )

kde4_add_plugin(kformdesigner_stdwidgets ${kformdesigner_stdwidgets_PART_SRCS})



target_link_libraries(
    kformdesigner_stdwidgets
    kformdesigner
    ${KOPROPERTY_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kformdesigner_stdwidgets  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformdesigner_containers.desktop kformdesigner_stdwidgets.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kformdesigner)
