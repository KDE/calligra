include_directories( ${KOFFICEUI_INCLUDES} )

########### Flake Plugin library ###############

SET (videoshape_LIB_SRCS
	VideoShape.cpp
	VideoShapeFactory.cpp
	VideoTool.cpp
	VideoToolFactory.cpp
	)

kde4_automoc(${videoshape_LIB_SRCS})
kde4_add_plugin(videoshape ${videoshape_LIB_SRCS})

target_link_libraries(videoshape flake kofficeui phononui)
install(TARGETS videoshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES videoshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
