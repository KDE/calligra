include_directories( ${KOFFICECORE_INCLUDES} ${FLAKE_INCLUDES} )

########### Flake Plugin library ###############
set(koregularpolygon_PART_SRCS
	KoRegularPolygonShape.cpp
	KoRegularPolygonShapeFactory.cpp
)

kde4_automoc(${koregularpolygon_PART_SRCS})

kde4_add_plugin(koregularpolygon ${koregularpolygon_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} koregularpolygon )

target_link_libraries(koregularpolygon flake kofficeui)

install(TARGETS koregularpolygon  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES regularpolygon.desktop DESTINATION ${SERVICES_INSTALL_DIR})
