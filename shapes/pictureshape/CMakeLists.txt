include_directories( ${KOFFICEUI_INCLUDES} )

########### Flake Plugin library ###############

SET (pictureshape_LIB_SRCS
    PictureShape.cpp
    PictureShapeFactory.cpp
    PictureTool.cpp
    PictureToolFactory.cpp
    Plugin.cpp
)

kde4_automoc(${pictureshape_LIB_SRCS})
kde4_add_plugin(pictureshape ${pictureshape_LIB_SRCS})

target_link_libraries(pictureshape flake kofficeui)
install(TARGETS pictureshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES pictureshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
