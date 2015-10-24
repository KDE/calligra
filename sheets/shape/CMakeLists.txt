add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_spreadsheet\")

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
kcoreaddons_desktop_to_json(calligra_shape_spreadsheet-deferred calligra_shape_spreadsheet-deferred.desktop)
add_library(calligra_shape_spreadsheet MODULE TableShapeFactory.cpp)
kcoreaddons_desktop_to_json(calligra_shape_spreadsheet calligra_shape_spreadsheet.desktop)

target_link_libraries(calligra_shape_spreadsheet-deferred calligrasheetscommon)
target_link_libraries(calligra_shape_spreadsheet flake KF5::KDELibs4Support)

install(TARGETS calligra_shape_spreadsheet-deferred DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(TARGETS calligra_shape_spreadsheet DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### install files ###############

ecm_install_icons( ICONS
    22-actions-spreadsheetshape.png
    22-actions-tool_cellformatting.png
    32-actions-spreadsheetshape.png

    DESTINATION ${DATA_INSTALL_DIR}/calligra/icons
    THEME hicolor
)
