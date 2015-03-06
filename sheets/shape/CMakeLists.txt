include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/sheets/ui
    ${KOMAIN_INCLUDES}
	${KOTEXT_INCLUDES}
    ${TEXTLAYOUT_INCLUDES}
	)

########### Flake Plugin library ###############

set (tableshape_LIB_SRCS
    TablePageManager.cpp
    TableShape.cpp
    TableShapeDeferredFactory.cpp
    TableTool.cpp
    TableToolFactory.cpp
    SheetsEditor.cpp
   )

add_library(calligra_shape_spreadsheet-deferred MODULE ${tableshape_LIB_SRCS})
add_library(calligra_shape_spreadsheet MODULE TableShapeFactory.cpp)

target_link_libraries(calligra_shape_spreadsheet-deferred calligrasheetscommon)
target_link_libraries(calligra_shape_spreadsheet flake)

install(TARGETS calligra_shape_spreadsheet-deferred DESTINATION ${PLUGIN_INSTALL_DIR})
install(TARGETS calligra_shape_spreadsheet DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES calligra_shape_spreadsheet-deferred.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install( FILES calligra_shape_spreadsheet.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
ecm_install_icons( ${DATA_INSTALL_DIR}/calligra/icons )
