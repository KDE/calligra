project(kspread)

add_subdirectory( flake )
add_subdirectory( dialogs )
add_subdirectory( tests )
add_subdirectory( plugins )
add_subdirectory( pics )
add_subdirectory( dtd )
add_subdirectory( toolbar )
add_subdirectory( functions )
add_subdirectory( sheetstyles )
add_subdirectory( templates )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/tests
                     ${CMAKE_CURRENT_SOURCE_DIR}/ui
                     ${CMAKE_SOURCE_DIR}/interfaces
                     ${CMAKE_SOURCE_DIR}/kchart
                     ${CMAKE_SOURCE_DIR}/kchart/kdchart/src
                     ${KOGUIUTILS_INCLUDES}
                     ${KOTEXT_INCLUDES}
                     ${EIGEN_INCLUDE_DIR} )


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
	commands/CSVDataCommand.cpp
	commands/DataManipulators.cpp
	commands/DefinePrintRangeCommand.cpp
	commands/DeleteCommand.cpp
# 	commands/EmbeddedObjectCommands.cpp
	commands/IndentationCommand.cpp
	commands/LinkCommand.cpp
	commands/MacroCommand.cpp
	commands/MergeCommand.cpp
	commands/NamedAreaCommand.cpp
	commands/PageLayoutCommand.cpp
	commands/PrecisionCommand.cpp
	commands/RowColumnManipulators.cpp
	commands/SheetCommands.cpp
	commands/SortManipulator.cpp
	commands/StyleCommand.cpp
	commands/Undo.cpp
	commands/UndoWrapperCommand.cpp
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
	dialogs/FindDialog.cpp
	dialogs/FormulaDialog.cpp
	dialogs/GoalSeekDialog.cpp
	dialogs/GotoDialog.cpp
	dialogs/InsertDialog.cpp
	dialogs/LayoutDialog.cpp
	dialogs/LinkDialog.cpp
	dialogs/ListDialog.cpp
	dialogs/NamedAreaDialog.cpp
	dialogs/PageLayoutDialog.cpp
	dialogs/PasteInsertDialog.cpp
	dialogs/PreferenceDialog.cpp
	dialogs/Resize2Dialog.cpp
	dialogs/SeriesDialog.cpp
	dialogs/SheetPropertiesDialog.cpp
	dialogs/SheetSelectPage.cpp
	dialogs/ShowDialog.cpp
	dialogs/ShowColRowDialog.cpp
	dialogs/SortDialog.cpp
	dialogs/SpecialPasteDialog.cpp
	dialogs/StyleManagerDialog.cpp
	dialogs/SubtotalDialog.cpp
	dialogs/ValidityDialog.cpp
	)


kde4_add_ui_files(dialogs_DIR_SRCS
	dialogs/FontWidget.ui
	dialogs/PageLayoutSheetPage.ui
	dialogs/PositionWidget.ui
	dialogs/ProtectionWidget.ui
	dialogs/SheetPropertiesWidget.ui
	dialogs/SheetSelectWidget.ui
	dialogs/SpecialPasteWidget.ui
	dialogs/SubtotalWidget.ui
	)

SET (functions_DIR_SRCS
	functions/bitops.cpp
	functions/conversion.cpp
	functions/database.cpp
	functions/datetime.cpp
	functions/engineering.cpp
	functions/financial.cpp
	functions/helper.cpp
	functions/information.cpp
	functions/logic.cpp
	functions/math.cpp
	functions/reference.cpp
	functions/statistical.cpp
	functions/text.cpp
	functions/trig.cpp
	)

SET (interfaces_DIR_SRCS
	interfaces/MapAdaptor.cpp
	interfaces/SheetAdaptor.cpp
	interfaces/ViewAdaptor.cpp
	)

SET (ui_DIR_SRCS
	ui/AbstractSelectionStrategy.cpp
	ui/AutoFillStrategy.cpp
	ui/Border.cpp
	ui/Canvas.cpp
	ui/CellToolBase.cpp
	ui/CellToolBase_p.cpp
	ui/CellView.cpp
	ui/DefaultTool.cpp
	ui/DefaultToolFactory.cpp
	ui/DragAndDropStrategy.cpp
	ui/Editors.cpp
	ui/MergeStrategy.cpp
	ui/PasteStrategy.cpp
	ui/PrintJob.cpp
	ui/SelectionStrategy.cpp
	ui/SheetView.cpp
	ui/View.cpp
	)

kde4_add_ui_files(ui_DIR_SRCS
	ui/CellToolOptionWidget.ui
	)

SET (kspreadcommon_LIB_SRCS
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
	Digest.cpp
	Doc.cpp
	Factory.cpp
	Format.cpp
	Formula.cpp
	Functions.cpp
# 	GeneralProperty.cpp
	GenValidationStyle.cpp
# 	Handler.cpp
	Localization.cpp
	Map.cpp
	NamedAreaManager.cpp
	Number.cpp
# 	Object.cpp
	PrintManager.cpp
	PrintSettings.cpp
# 	PropertyEditor.cpp
	RecalcManager.cpp
	Region.cpp
	RowColumnFormat.cpp
	Selection.cpp
	ShapeApplicationData.cpp
	Sheet.cpp
	SheetPrint.cpp
	RectStorage.cpp
	Style.cpp
	StyleManager.cpp
	StyleStorage.cpp
# 	Toolbox.cpp
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
	${ui_DIR_SRCS}
	)

kde4_add_library(kspreadcommon SHARED ${kspreadcommon_LIB_SRCS})

target_link_libraries(kspreadcommon
	${QT_QTSQL_LIBRARY}
	${KDE4_KNOTIFYCONFIG_LIBS}
	komain koguiutils kochart
	${KDE4_KDE3SUPPORT_LIBS} )

set_target_properties(kspreadcommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kspreadcommon  DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

SET (kspreadpart_PART_SRCS Factory_init.cpp )

kde4_add_plugin(kspreadpart WITH_PREFIX ${kspreadpart_PART_SRCS})



target_link_libraries(kspreadpart  ${KDE4_KDECORE_LIBS} kspreadcommon )

install(TARGETS kspreadpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

SET (kspread_KDEINIT_SRCS Main.cpp )

kde4_add_kdeinit_executable( kspread ${kspread_KDEINIT_SRCS})

target_link_libraries(kdeinit_kspread  ${KDE4_KDECORE_LIBS} komain )

install(TARGETS kdeinit_kspread  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kspread kdeinit_kspread )
install(TARGETS kspread  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  kspread.rc kspread_readonly.rc kspread.notifyrc DESTINATION ${DATA_INSTALL_DIR}/kspread)
install( FILES  kspreadpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kspread.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  kspread.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
