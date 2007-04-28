add_subdirectory( fonts )
add_subdirectory( core )

include_directories( ${KOFFICEUI_INCLUDES} )

########### Flake Plugin library ###############

SET (musicshape_LIB_SRCS
	MusicShape.cpp
	MusicShapeFactory.cpp
	MusicTool.cpp
	MusicToolFactory.cpp
    MusicStyle.cpp
	)

kde4_automoc(${musicshape_LIB_SRCS})
kde4_add_plugin(musicshape ${musicshape_LIB_SRCS})

target_link_libraries(musicshape flake kofficeui musiccore)
install(TARGETS musicshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES musicshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
