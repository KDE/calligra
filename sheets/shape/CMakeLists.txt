add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_spreadsheet\")

include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_BINARY_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/sheets/ui
    ${CMAKE_SOURCE_DIR}/sheets/database
    ${CMAKE_SOURCE_DIR}/sheets/part
    ${CMAKE_SOURCE_DIR}/sheets/commands
    ${KOMAIN_INCLUDES}
    ${KOTEXT_INCLUDES}
    ${TEXTLAYOUT_INCLUDES}
)

########### Flake Plugin library ###############

set (tableshape_LIB_SRCS
    TableShape.cpp
    TableTool.cpp
    TableShapeFactory.cpp
    TableToolFactory.cpp
    SheetsEditor.cpp
    ToolHeaders.cpp
    TableShapeConfigWidget.cpp
)

ki18n_wrap_ui(tableshape_LIB_SRCS
    SheetsEditor.ui
    TableShapeConfigWidget.ui
)

add_library(calligra_shape_spreadsheet MODULE ${tableshape_LIB_SRCS})
calligra_shape_desktop_to_json(calligra_shape_spreadsheet calligra_shape_spreadsheet.desktop)

target_link_libraries(calligra_shape_spreadsheet
    PUBLIC
        calligrasheetscommon
        calligrasheetspart
        flake
        komain
    PRIVATE
        Qt5::PrintSupport
        KF5::Completion
)

install(TARGETS calligra_shape_spreadsheet DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/shapes)

########### install files ###############

ecm_install_icons( ICONS
    22-actions-spreadsheetshape.png
    22-actions-tool_cellformatting.png
    32-actions-spreadsheetshape.png

    DESTINATION ${DATA_INSTALL_DIR}/calligra/icons
    THEME hicolor
)
