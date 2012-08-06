project(calligra-sheets)

add_subdirectory( data )
add_subdirectory( shape )
add_subdirectory( tests )
add_subdirectory( plugins )
add_subdirectory( dtd )
add_subdirectory( functions )

#add_definitions(-DCALLIGRA_SHEETS_MT)

include_directories( ${CMAKE_SOURCE_DIR}/interfaces
                    ${CMAKE_SOURCE_DIR}/kchart
                    ${KOMAIN_INCLUDES}
                    ${KOTEXT_INCLUDES}
                    ${TEXTLAYOUT_INCLUDES}
                    ${Boost_INCLUDE_DIR}
                    ${EIGEN2_INCLUDE_DIR} )

########### next target ###############
SET (chart_DIR_SRCS
    chart/ChartDatabaseSelectorFactory.cpp
    chart/ChartDatabaseSelector.cpp
    chart/ChartDialog.cpp
    )

kde4_add_ui_files(chart_DIR_SRCS
    chart/ChartDatabaseSelector.ui
    )

SET (commands_DIR_SRCS
    commands/AbstractRegionCommand.cpp
    commands/ApplyFilterCommand.cpp
    commands/AutoFillCommand.cpp
    commands/AutoFilterCommand.cpp
    commands/AutoFormatCommand.cpp
    commands/BorderColorCommand.cpp
    commands/CommentCommand.cpp
    commands/ConditionCommand.cpp
    commands/CopyCommand.cpp
    commands/CSVDataCommand.cpp
    commands/DataManipulators.cpp
    commands/DeleteCommand.cpp
    commands/IndentationCommand.cpp
    commands/LinkCommand.cpp
    commands/MergeCommand.cpp
    commands/NamedAreaCommand.cpp
    commands/PageBreakCommand.cpp
    commands/PasteCommand.cpp
    commands/PrecisionCommand.cpp
    commands/RowColumnManipulators.cpp
    commands/SheetCommands.cpp
    commands/SortManipulator.cpp
    commands/SpellCheckCommand.cpp
    commands/StyleCommand.cpp
    commands/ValidityCommand.cpp
    )

SET (database_DIR_SRCS
    #database/Database.cpp
    #database/DatabaseManager.cpp
    database/DatabaseSource.cpp
    database/DatabaseSourceQuery.cpp
    database/DatabaseSourceSql.cpp
    database/DatabaseSourceTable.cpp
    #database/Filter.cpp
    database/FilterPopup.cpp
    )

SET (dialogs_DIR_SRCS
    dialogs/AddNamedAreaDialog.cpp
    dialogs/AngleDialog.cpp
    dialogs/AutoFormatDialog.cpp
    dialogs/CharacterSelectDialog.cpp
    dialogs/CommentDialog.cpp
    dialogs/ConditionalDialog.cpp
    dialogs/ConsolidateDialog.cpp
    dialogs/CSVDialog.cpp
    dialogs/DatabaseDialog.cpp
    dialogs/DocumentSettingsDialog.cpp
    dialogs/FindDialog.cpp
    dialogs/FormulaDialog.cpp
    dialogs/GoalSeekDialog.cpp
    dialogs/GotoDialog.cpp
    dialogs/InsertDialog.cpp
    dialogs/LayoutDialog.cpp
    dialogs/LinkDialog.cpp
    dialogs/ListDialog.cpp
    dialogs/NamedAreaDialog.cpp
    dialogs/PasteInsertDialog.cpp
    dialogs/Resize2Dialog.cpp
    dialogs/SeriesDialog.cpp
    dialogs/ShowDialog.cpp
    dialogs/ShowColRowDialog.cpp
    dialogs/SortDialog.cpp
    dialogs/SpecialPasteDialog.cpp
    dialogs/StyleManagerDialog.cpp
    dialogs/SubtotalDialog.cpp
    dialogs/ValidityDialog.cpp
    )


kde4_add_ui_files(dialogs_DIR_SRCS
    dialogs/ConsolidateWidget.ui
    dialogs/ConsolidateDetailsWidget.ui
    dialogs/FontWidget.ui
    dialogs/GoalSeekWidget.ui
    dialogs/PositionWidget.ui
    dialogs/ProtectionWidget.ui
    dialogs/SpecialPasteWidget.ui
    dialogs/SortWidget.ui
    dialogs/SortDetailsWidget.ui
    dialogs/SubtotalWidget.ui
    dialogs/SubtotalsDetailsWidget.ui
    )

SET (functions_DIR_SRCS
    functions/helper.cpp
    )

SET (interfaces_DIR_SRCS
    interfaces/MapAdaptor.cpp
    interfaces/SheetAdaptor.cpp
    interfaces/ViewAdaptor.cpp
    )

SET (part_DIR_SRCS
    part/CanvasBase.cpp
    part/Canvas.cpp
    part/CanvasItem.cpp
    part/CellTool.cpp
    part/CellToolFactory.cpp
    #part/Digest.cpp
    part/Doc.cpp
    part/Part.cpp
    part/Factory.cpp
    part/Find.cpp
    part/Headers.cpp
    part/HeaderWidgets.cpp
    part/HeaderItems.cpp
    part/PrintJob.cpp
    part/ToolRegistry.cpp
    part/TabBar.cpp
    part/View.cpp
    part/commands/DefinePrintRangeCommand.cpp
    part/commands/PageLayoutCommand.cpp
    part/dialogs/PageLayoutDialog.cpp
    part/dialogs/PreferenceDialog.cpp
    part/dialogs/SheetPropertiesDialog.cpp
    part/dialogs/SheetSelectPage.cpp
)

kde4_add_ui_files(part_DIR_SRCS
    part/dialogs/FileOptionsWidget.ui
    part/dialogs/InterfaceOptionsWidget.ui
    part/dialogs/PageLayoutSheetPage.ui
    part/dialogs/SheetPropertiesWidget.ui
    part/dialogs/SheetSelectWidget.ui
)

SET (ui_DIR_SRCS
    ui/AbstractSelectionStrategy.cpp
    ui/ActionOptionWidget.cpp
    ui/AutoFillStrategy.cpp
    ui/CellEditorBase.cpp
    ui/CellEditor.cpp
    ui/CellEditorDocker.cpp
    ui/CellToolBase.cpp
    ui/CellToolBase_p.cpp
    ui/CellView.cpp
    ui/DragAndDropStrategy.cpp
    ui/FormulaEditorHighlighter.cpp
    ui/FunctionCompletion.cpp
    ui/ExternalEditor.cpp
    ui/HyperlinkStrategy.cpp
    ui/LocationComboBox.cpp
    ui/MapViewModel.cpp
    ui/MergeStrategy.cpp
    ui/PasteStrategy.cpp
    ui/PixmapCachingSheetView.cpp
    ui/RegionSelector.cpp
    ui/RightToLeftPaintingStrategy.cpp
    ui/Selection.cpp
    ui/SelectionStrategy.cpp
    ui/SheetView.cpp
)

SET (calligrasheetscommon_LIB_SRCS
    MapModel.cpp
    PageManager.cpp
    RegionModel.cpp
    tests/inspector.cpp
    ${chart_DIR_SRCS}
    ${commands_DIR_SRCS}
    ${database_DIR_SRCS}
    ${dialogs_DIR_SRCS}
    ${functions_DIR_SRCS}
    ${interfaces_DIR_SRCS}
    ${part_DIR_SRCS}
    ${ui_DIR_SRCS}
    )

set (calligrasheetsodf_LIB_SRCS
    part/Digest.cpp
    ApplicationSettings.cpp
    Binding.cpp
    BindingManager.cpp
    CalculationSettings.cpp
    Cell.cpp
    CellStorage.cpp
    Cluster.cpp
    Condition.cpp
    Currency.cpp
    Damages.cpp
    DependencyManager.cpp
    DocBase.cpp
    Format.cpp
    Formula.cpp
    GenValidationStyle.cpp
    HeaderFooter.cpp
    Localization.cpp
    Map.cpp
    NamedAreaManager.cpp
    Number.cpp
    PrintSettings.cpp
    ProtectableObject.cpp
    RecalcManager.cpp
    RectStorage.cpp
    Region.cpp
    RowColumnFormat.cpp
    RowFormatStorage.cpp
    RowRepeatStorage.cpp
    ShapeApplicationData.cpp
    Sheet.cpp
    SheetAccessModel.cpp
    SheetModel.cpp
    Style.cpp
    StyleManager.cpp
    StyleStorage.cpp
    Util.cpp
    Validity.cpp
    Value.cpp
    ValueCalc.cpp
    ValueConverter.cpp
    ValueFormatter.cpp
    ValueParser.cpp

    database/Database.cpp
    database/DatabaseManager.cpp
    database/Filter.cpp

    # TODO: move the formula evaluation out of Formula.cpp so these files can move out of libcalligrasheetsodf
    Function.cpp
    FunctionDescription.cpp
    FunctionModule.cpp
    FunctionModuleRegistry.cpp
    FunctionRepository.cpp

    # TODO: move HeaderFooter from SheetPrint to PrintSettings, and replace SheetPrint with PrintSettings in Sheet to get rid of this dependency
    SheetPrint.cpp
    SheetPrint_p.cpp
    )


kde4_add_library(calligrasheetsodf SHARED ${calligrasheetsodf_LIB_SRCS})

target_link_libraries(calligrasheetsodf komain)
target_link_libraries(calligrasheetsodf LINK_INTERFACE_LIBRARIES komain)

set_target_properties(calligrasheetsodf PROPERTIES
   VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetsodf ${INSTALL_TARGETS_DEFAULT_ARGS})


kde4_add_library(calligrasheetscommon SHARED ${calligrasheetscommon_LIB_SRCS})

target_link_libraries(calligrasheetscommon
    komain
    calligrasheetsodf
    ${QT_QTSQL_LIBRARY}
    ${KDE4_KNOTIFYCONFIG_LIBS}
    ${KDE4_THREADWEAVER_LIBS}
#    ${KDE4_KDE3SUPPORT_LIBS}
)
if(KDE4_KCMUTILS_LIBS)
    target_link_libraries(calligrasheetscommon ${KDE4_KCMUTILS_LIBS} )
else()
    target_link_libraries(calligrasheetscommon ${KDE4_KUTILS_LIBS} )
endif()

target_link_libraries(calligrasheetscommon LINK_INTERFACE_LIBRARIES komain calligrasheetsodf )

set_target_properties(calligrasheetscommon PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetscommon  ${INSTALL_TARGETS_DEFAULT_ARGS})
########### next target ###############

SET (calligrasheetspart_PART_SRCS part/Factory_init.cpp )

kde4_add_plugin(calligrasheetspart ${calligrasheetspart_PART_SRCS})



target_link_libraries(calligrasheetspart calligrasheetscommon )

install(TARGETS calligrasheetspart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

SET (calligrasheets_KDEINIT_SRCS part/Main.cpp )

kde4_add_app_icon(calligrasheets_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/data/pics/hi*-app-sheets.png")

kde4_add_kdeinit_executable( calligrasheets ${calligrasheets_KDEINIT_SRCS})

if (Q_WS_MAC)
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.sheets")
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Calligra Sheets 2")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_calligrasheets komain )

install(TARGETS kdeinit_calligrasheets  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligrasheets kdeinit_calligrasheets )
install(TARGETS calligrasheets  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  sheets.rc sheets_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/sheets)
install( FILES  ui/CellToolOptionWidgets.xml DESTINATION ${DATA_INSTALL_DIR}/sheets)
install( FILES  sheetspart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS  sheets.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  sheets.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
install( FILES  sheets_plugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install( FILES  sheetsrc DESTINATION ${CONFIG_INSTALL_DIR})


install( FILES
    calligra_sheets_export.h
    calligra_sheets_limits.h

    Cell.h
    CellStorage.h
    Condition.h
    Currency.h
    DocBase.h
    Format.h
    Global.h
    Map.h
    Number.h
    OdfLoadingContext.h
    PointStorage.h
    PrintSettings.h
    ProtectableObject.h
    RectStorage.h
    Region.h
    RowColumnFormat.h
    RowFormatStorage.h
    RTree.h
    Sheet.h
    Style.h
    Value.h
    ValueCalc.h
    ValueConverter.h
    ValueStorage.h
DESTINATION ${INCLUDE_INSTALL_DIR}/sheets COMPONENT Devel)
install( FILES
    part/CanvasBase.h
    part/CanvasItem.h
    part/CellTool.h
    part/Doc.h
    part/Find.h
    part/HeaderItems.h
    part/Headers.h
    part/ToolRegistry.h
    part/View.h
DESTINATION ${INCLUDE_INSTALL_DIR}/sheets/part COMPONENT Devel)
install( FILES
    ui/CellToolBase.h
    ui/CellEditorBase.h
    ui/Selection.h
    ui/SheetView.h
DESTINATION ${INCLUDE_INSTALL_DIR}/sheets/ui COMPONENT Devel)
install( FILES
    database/Database.h
    database/Filter.h
DESTINATION ${INCLUDE_INSTALL_DIR}/sheets/database COMPONENT Devel)
install( FILES
    commands/AbstractRegionCommand.h
    commands/DataManipulators.h
    commands/SortManipulator.h
DESTINATION ${INCLUDE_INSTALL_DIR}/sheets/commands COMPONENT Devel)
