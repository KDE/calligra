include_directories(
	${CMAKE_SOURCE_DIR}/kspread
	${CMAKE_SOURCE_DIR}/kspread/ui
	${KOGUIUTILS_INCLUDES}
	)

########### Flake Plugin library ###############

SET (tableshape_LIB_SRCS
    TableShape.cpp
    TableShapeFactory.cpp
    TableTool.cpp
    TableToolFactory.cpp
    SheetsEditor.cpp
    )

kde4_add_plugin(spreadsheetshape ${tableshape_LIB_SRCS})

target_link_libraries(spreadsheetshape kspreadcommon flake)
install(TARGETS spreadsheetshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES spreadsheetshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
