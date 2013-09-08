include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/sheets/ui
    ${KOMAIN_INCLUDES}
	${KOTEXT_INCLUDES}
    ${TEXTLAYOUT_INCLUDES}
	)

########### Flake Plugin library ###############

SET (tableshape_LIB_SRCS
    TablePageManager.cpp
    TableShape.cpp
    TableShapeDeferredFactory.cpp
    TableTool.cpp
    TableToolFactory.cpp
    SheetsEditor.cpp
   )

kde4_add_plugin(calligra_shape_spreadsheet-deferred ${tableshape_LIB_SRCS})
kde4_add_plugin(calligra_shape_spreadsheet TableShapeFactory.cpp)

target_link_libraries(calligra_shape_spreadsheet-deferred calligrasheetscommon)
target_link_libraries(calligra_shape_spreadsheet flake)

install(TARGETS calligra_shape_spreadsheet-deferred DESTINATION ${PLUGIN_INSTALL_DIR})
install(TARGETS calligra_shape_spreadsheet DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES calligra_shape_spreadsheet-deferred.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES calligra_shape_spreadsheet.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/calligra/icons )
