add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_spreadsheet\")

########### Flake Plugin library ###############

set (tableshape_LIB_SRCS
    PageManager.cpp
    TablePageManager.cpp
    TableShape.cpp
    TableTool.cpp
    TableShapeFactory.cpp
    TableToolFactory.cpp
    SheetsEditor.cpp
    ToolHeaders.cpp
    ScreenConversions.cpp
   )

add_library(calligra_shape_spreadsheet MODULE ${tableshape_LIB_SRCS})

target_include_directories( calligra_shape_spreadsheet
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)

target_link_libraries(calligra_shape_spreadsheet
    PUBLIC
        calligrasheetsengine
        calligrasheetscore
        calligrasheetsui
        flake
        komain
    PRIVATE
        Qt6::PrintSupport
        KF6::Completion
)
# target_link_libraries(calligra_shape_spreadsheet flake)

# install(TARGETS calligra_shape_spreadsheet-deferred DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/deferred)
install(TARGETS calligra_shape_spreadsheet DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)

########### install files ###############

ecm_install_icons( ICONS
    22-actions-spreadsheetshape.png
    22-actions-tool_cellformatting.png
    32-actions-spreadsheetshape.png

    DESTINATION ${KDE_INSTALL_DATADIR}/calligra/icons
    THEME hicolor
)
