include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${KOFFICE_INCLUDE_DIR} )

########### Flake Plugin library ###############

SET (webshape_LIB_SRCS
    WebShape.cpp
    WebShapeFactory.cpp
    WebShapePlugin.cpp
    WebTool.cpp
    WebToolWidget.cpp
    WebToolFactory.cpp
)

kde4_add_ui_files( webshape_LIB_SRCS
    WebShapeConfigWidget.ui
)

kde4_add_plugin(webshape ${webshape_LIB_SRCS})

target_link_libraries(
    webshape
    ${KOMAIN_LIBRARY}
    ${KOODF_LIBRARY}
    ${FLAKE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KFILE_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTWEBKIT_LIBRARY}
    )
install(TARGETS webshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES webshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )

