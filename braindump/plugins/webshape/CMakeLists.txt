include_directories (${FLAKE_INCLUDES} )

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
    flake
    ${QT_QTCORE_LIBRARY}
    ${QT_QTWEBKIT_LIBRARY}
    )
install(TARGETS webshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES webshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/calligra/icons )

