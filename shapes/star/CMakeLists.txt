include_directories( ${KOFFICECORE_INCLUDES} ${FLAKE_INCLUDES} )

########### Flake Plugin library ###############
set(kostarshape_PART_SRCS
	KoStarShape.cpp
	KoStarShapeFactory.cpp
)

kde4_automoc(${kostarshape_PART_SRCS})

kde4_add_plugin(kostarshape ${kostarshape_PART_SRCS})



target_link_libraries(kostarshape flake kofficeui)

install(TARGETS kostarshape  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES starshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
