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

kde4_add_plugin(spreadsheetshape-deferred ${tableshape_LIB_SRCS})
kde4_add_plugin(spreadsheetshape TableShapeFactory.cpp)

target_link_libraries(spreadsheetshape-deferred calligrasheetscommon)
target_link_libraries(spreadsheetshape flake)

install(TARGETS spreadsheetshape-deferred DESTINATION ${PLUGIN_INSTALL_DIR})
install(TARGETS spreadsheetshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES spreadsheetshape-deferred.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES spreadsheetshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/calligra/icons )
