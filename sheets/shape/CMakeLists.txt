add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_spreadsheet\")

include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_BINARY_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/sheets/ui
    ${CMAKE_SOURCE_DIR}/sheets/database
    ${CMAKE_SOURCE_DIR}/sheets/commands
    ${KOMAIN_INCLUDES}
    ${KOTEXT_INCLUDES}
    ${TEXTLAYOUT_INCLUDES}
)

########### Flake Plugin library ###############

set (tableshape_LIB_SRCS
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
calligra_shape_desktop_to_json(calligra_shape_spreadsheet calligra_shape_spreadsheet.desktop)

target_link_libraries(calligra_shape_spreadsheet
    PUBLIC
        calligrasheetscommon
        flake
        komain
    PRIVATE
        Qt5::PrintSupport
        KF5::Completion
)
# target_link_libraries(calligra_shape_spreadsheet flake KF5::KDELibs4Support)

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
