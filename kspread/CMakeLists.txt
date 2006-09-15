project(kspread)

#add_subdirectory( rtree )
add_subdirectory( dialogs )
add_subdirectory( tests )
add_subdirectory( plugins )
add_subdirectory( pics )
add_subdirectory( dtd )
add_subdirectory( toolbar )
add_subdirectory( functions )
add_subdirectory( sheetstyles )
add_subdirectory( templates )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/kchart/kdchart ${KOTEXT_INCLUDES} ${KOFFICEUI_INCLUDES} )


########### next target ###############
SET (dialogs_DIR_SRCS
	dialogs/AngleDialog.cpp
	dialogs/AreaDialog.cpp
	dialogs/CommentDialog.cpp
	dialogs/ConditionalDialog.cpp
	dialogs/ConsolidateDialog.cpp
	dialogs/CSVDialog.cpp
	dialogs/DatabaseDialog.cpp
	dialogs/FindDialog.cpp
	dialogs/FormatDialog.cpp
	dialogs/FormulaDialog.cpp
	dialogs/GoalSeekDialog.cpp
	dialogs/GotoDialog.cpp
	dialogs/InsertDialog.cpp
	dialogs/LayoutDialog.cpp
	dialogs/LinkDialog.cpp
	dialogs/ListDialog.cpp
	dialogs/PaperLayoutDialog.cpp
	dialogs/PasteInsertDialog.cpp
	dialogs/PreferenceDialog.cpp
	dialogs/ReferenceDialog.cpp
	dialogs/Resize2Dialog.cpp
	dialogs/SeriesDialog.cpp
	dialogs/SheetPropertiesDialog.cpp
	dialogs/SheetSelectPage.cc
	dialogs/ShowDialog.cpp
	dialogs/ShowColRowDialog.cpp
	dialogs/SortDialog.cpp
	dialogs/SpecialDialog.cpp
	dialogs/StylesDialog.cpp
	dialogs/SubtotalDialog.cpp
	dialogs/ValidityDialog.cpp
	)


kde4_add_ui_files(dialogs_DIR_SRCS
	dialogs/FontWidget.ui
	dialogs/PositionWidget.ui
	dialogs/ProtectionWidget.ui
	dialogs/SheetPropertiesWidget.ui
	dialogs/SheetSelectWidget.ui
	dialogs/SpecialPasteWidget.ui
	dialogs/SubtotalWidget.ui
	)


SET (tests_DIR_SRCS
	tests/tester.cc
	tests/testrunner.cc
	tests/value_tester.cc
	tests/formula_tester.cc
	tests/inspector.cc
	)

SET (functions_DIR_SRCS
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

SET (kspreadcommon_LIB_SRCS
	AutoFill.cpp
	Border.cpp
	Canvas.cpp
	Cell.cpp
	CellView.cpp
	Cluster.cpp
	Commands.cpp
	Condition.cpp
	Damages.cpp
	DataManipulators.cpp
	DependencyManager.cpp
	Digest.cpp
	Doc.cpp
	Editors.cpp
	Events.cpp
	Factory.cpp
	Format.cpp
	FormatManipulators.cpp
	Formula.cpp
	Functions.cpp
	GeneralProperty.cpp
	GenValidationStyle.cpp
	Handler.cpp
	Localization.cpp
	Manipulator.cpp
	Map.cpp
	MapAdaptor.cpp
# 	Numformat.cpp
	Object.cpp
	PropertyEditor.cpp
	RecalcManager.cpp
	Region.cpp
	RowColumnManipulators.cpp
	Selection.cpp
	Sheet.cpp
	SheetAdaptor.cpp
	SheetPrint.cpp
	SortManipulator.cpp
	Style.cpp
	StyleManager.cpp
	Toolbox.cpp
	Undo.cpp
	Util.cpp
	Validity.cpp
	Value.cpp
	ValueCalc.cpp
	ValueConverter.cpp
	ValueFormatter.cpp
	ValueParser.cpp
	View.cpp
	ViewAdaptor.cpp
	${dialogs_DIR_SRCS}
	${tests_DIR_SRCS}
	${functions_DIR_SRCS}
	)

kde4_automoc(${kspreadcommon_LIB_SRCS})

kde4_add_library(kspreadcommon SHARED ${kspreadcommon_LIB_SRCS})

target_link_libraries(kspreadcommon
	${QT_QTSQL_LIBRARY}
	kofficecore kofficeui kochart
	${KDE4_KDE3SUPPORT_LIBS}
	${KDE4_KSPELL2_LIBS} )

set_target_properties(kspreadcommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kspreadcommon  DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

SET (kspreadpart_PART_SRCS Factory_init.cpp )

kde4_automoc(${kspreadpart_PART_SRCS})

kde4_add_plugin(kspreadpart WITH_PREFIX ${kspreadpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadpart )

target_link_libraries(kspreadpart  ${KDE4_KDECORE_LIBS} kspreadcommon )

install(TARGETS kspreadpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

SET (kspread_KDEINIT_SRCS Main.cpp )

kde4_automoc(${kspread_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kspread ${kspread_KDEINIT_SRCS})

target_link_libraries(kdeinit_kspread  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kspread  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kspread kdeinit_kspread )
install(TARGETS kspread  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kspread.rc kspread_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread)
install( FILES  kspreadpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kspread.desktop  DESTINATION ${XDG_APPS_DIR})

