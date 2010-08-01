project(kspread)

add_subdirectory( data )
add_subdirectory( shape )
add_subdirectory( tests )
add_subdirectory( plugins )
add_subdirectory( dtd )
add_subdirectory( functions )

include_directories( ${CMAKE_SOURCE_DIR}/interfaces
                    ${CMAKE_SOURCE_DIR}/kchart
                    ${KOMAIN_INCLUDES}
                    ${KOTEXT_INCLUDES}
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
        commands/ColumnStyleCommand.cpp
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
        commands/RowStyleCommand.cpp
    commands/SheetCommands.cpp
    commands/SortManipulator.cpp
    commands/SpellCheckCommand.cpp
    commands/StyleCommand.cpp
    commands/ValidityCommand.cpp
    )

SET (database_DIR_SRCS
    database/Database.cpp
    database/DatabaseManager.cpp
    database/DatabaseSource.cpp
    database/DatabaseSourceQuery.cpp
    database/DatabaseSourceSql.cpp
    database/DatabaseSourceTable.cpp
    database/Filter.cpp
    database/FilterPopup.cpp
    )

SET (dialogs_DIR_SRCS
    dialogs/AddNamedAreaDialog.cpp
    dialogs/AngleDialog.cpp
    dialogs/AutoFormatDialog.cpp
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
    part/Digest.cpp
    part/Doc.cpp
    part/Factory.cpp
    part/Headers.cpp
    part/HeaderWidgets.cpp
    part/HeaderItems.cpp
    part/PrintJob.cpp
    part/PrintManager.cpp
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
    ui/AutoFillStrategy.cpp
    ui/CellEditorBase.cpp
    ui/CellEditor.cpp
    ui/CellToolBase.cpp
    ui/CellToolBase_p.cpp
    ui/CellToolOptionWidget.cpp
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
    ui/RegionSelector.cpp
    ui/RightToLeftPaintingStrategy.cpp
    ui/SelectionStrategy.cpp
    ui/SheetView.cpp
    ui/CharSelectDia.cpp
)

SET (kspreadcommon_LIB_SRCS
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
    Format.cpp
    Formula.cpp
    Function.cpp
    FunctionDescription.cpp
    FunctionModule.cpp
    FunctionModuleRegistry.cpp
    FunctionRepository.cpp
    GenValidationStyle.cpp
    HeaderFooter.cpp
    Localization.cpp
    Map.cpp
    MapModel.cpp
    SheetAccessModel.cpp
    NamedAreaManager.cpp
    Number.cpp
    PageManager.cpp
    PrintSettings.cpp
    ProtectableObject.cpp
    RecalcManager.cpp
    Region.cpp
    RegionModel.cpp
    RowColumnFormat.cpp
    Selection.cpp
    ShapeApplicationData.cpp
    Sheet.cpp
    SheetModel.cpp
    SheetPrint.cpp
    SheetPrint_p.cpp
    RectStorage.cpp
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

kde4_add_library(kspreadcommon SHARED ${kspreadcommon_LIB_SRCS})

target_link_libraries(kspreadcommon
    komain
    ${QT_QTSQL_LIBRARY}
    ${KDE4_KNOTIFYCONFIG_LIBS}
    ${KDE4_KUTILS_LIBS}
#    ${KDE4_KDE3SUPPORT_LIBS}
)

target_link_libraries(kspreadcommon LINK_INTERFACE_LIBRARIES komain )

set_target_properties(kspreadcommon PROPERTIES
    VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION}
)
install(TARGETS kspreadcommon  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

SET (kspreadpart_PART_SRCS part/Factory_init.cpp )

kde4_add_plugin(kspreadpart WITH_PREFIX ${kspreadpart_PART_SRCS})



target_link_libraries(kspreadpart kspreadcommon )

install(TARGETS kspreadpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

SET (kspread_KDEINIT_SRCS part/Main.cpp )

kde4_add_app_icon(kspread_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/data/pics/hi*-app-kspread.png")

kde4_add_kdeinit_executable( kspread ${kspread_KDEINIT_SRCS})

if (Q_WS_MAC)
set_target_properties(kspread PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
set_target_properties(kspread PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.koffice.kspread")
set_target_properties(kspread PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KSpread 2")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_kspread komain )

install(TARGETS kdeinit_kspread  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( kspread kdeinit_kspread )
install(TARGETS kspread  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  kspread.rc kspread_readonly.rc kspread.notifyrc DESTINATION ${DATA_INSTALL_DIR}/kspread)
install( FILES  kspreadpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kspread.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  kspread.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
install( FILES  kspread_plugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install( FILES  kspreadrc DESTINATION ${CONFIG_INSTALL_DIR})
