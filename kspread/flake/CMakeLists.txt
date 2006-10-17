include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KOFFICEUI_INCLUDES} )

########### Flake Plugin library ###############

SET (tableshape_LIB_SRCS
	TableShape.cpp
	TableShapeFactory.cpp
	TableTool.cpp
	TableToolFactory.cpp
	)

kde4_automoc(${tableshape_LIB_SRCS})
kde4_add_plugin(tableshape ${tableshape_LIB_SRCS})
kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} tableshape )
target_link_libraries(tableshape kspreadcommon flake)
install(TARGETS tableshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES tableshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
