include_directories( ${KOFFICEUI_INCLUDES} )

########### Flake Plugin library ###############

SET (videoflake_LIB_SRCS
	VideoShape.cpp
	VideoShapeFactory.cpp
	VideoTool.cpp
	VideoToolFactory.cpp
	)

kde4_automoc(${videoflake_LIB_SRCS})
kde4_add_plugin(videoflake ${videoflake_LIB_SRCS})

target_link_libraries(videoflake flake kofficeui phononui)
install(TARGETS videoflake DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES videoflake.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
