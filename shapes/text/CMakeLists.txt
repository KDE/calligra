include_directories( ${KOFFICEUI_INCLUDES} ${CMAKE_SOURCE_DIR}/libs/kotext ${CMAKE_SOURCE_DIR}/libs/kotext/styles )

########### Flake Plugin library ###############

SET ( textshape_SRCS
    KoTextPlugin.cpp
    KoTextShape.cpp
    KoTextShapeFactory.cpp
    KoTextTool.cpp
    KoTextToolFactory.cpp
    Layout.cpp
    ListItemsHelper.cpp
)

kde4_automoc(${textshape_SRCS})
kde4_add_plugin(textshape ${textshape_SRCS})

target_link_libraries(textshape flake kofficeui kotext)
install(TARGETS textshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES textshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
#kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
