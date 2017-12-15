project(calligra-sheets)

# TEMPORARY: for  Qt5/KF5 build porting phase deprecation warnings are only annoying noise
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-deprecated -Wno-deprecated-declarations)
endif ()

include_directories( ${CMAKE_SOURCE_DIR}/interfaces
                    ${KOMAIN_INCLUDES}
                    ${KOTEXT_INCLUDES}
                    ${TEXTLAYOUT_INCLUDES}
                    ${Boost_INCLUDE_DIR}
                    ${EIGEN3_INCLUDE_DIR} )

if (SHOULD_BUILD_PART_SHEETS)

# have their own translation domain
add_subdirectory( shape )
add_subdirectory( plugins )

add_definitions(-DTRANSLATION_DOMAIN=\"calligrasheets\")

add_subdirectory( data )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
add_subdirectory( dtd )
add_subdirectory( functions )

#add_definitions(-DCALLIGRA_SHEETS_MT)

if(NOT Qt5Sql_FOUND)
    add_definitions(-DQT_NO_SQL)
endif()

########### next target ###############
set (chart_DIR_SRCS
    chart/ChartDatabaseSelectorFactory.cpp
    chart/ChartDatabaseSelector.cpp
    chart/ChartDialog.cpp
    )

ki18n_wrap_ui(chart_DIR_SRCS
    chart/ChartDatabaseSelector.ui
    )

set (commands_DIR_SRCS
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

set (database_DIR_SRCS
    #database/Database.cpp
    #database/DatabaseManager.cpp
    database/DatabaseSource.cpp
    database/DatabaseSourceQuery.cpp
    database/DatabaseSourceSql.cpp
    database/DatabaseSourceTable.cpp
    #database/Filter.cpp
    database/FilterPopup.cpp
    )

set (dialogs_DIR_SRCS
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
    dialogs/pivot.cpp
    dialogs/pivotfilters.cpp
    dialogs/pivotoptions.cpp
    dialogs/pivotmain.cpp

    )


ki18n_wrap_ui(dialogs_DIR_SRCS
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
    dialogs/pivot.ui
    dialogs/pivotfilters.ui
    dialogs/pivotoptions.ui
    dialogs/pivotmain.ui

    )

set (functions_DIR_SRCS
    functions/helper.cpp
    )

if(Qt5DBus_FOUND)
set (interfaces_DIR_SRCS
    interfaces/MapAdaptor.cpp
    interfaces/SheetAdaptor.cpp
    interfaces/ViewAdaptor.cpp
    )
endif()

set (odf_DIR_SRCS
    odf/SheetsOdfDoc.cpp
    odf/SheetsOdfMap.cpp
    odf/SheetsOdfSheet.cpp
    odf/SheetsOdfCell.cpp
    odf/SheetsOdfStyle.cpp
    odf/SheetsOdfRegion.cpp
    odf/SheetsOdfCondition.cpp
    odf/SheetsOdfValidity.cpp
    odf/GenValidationStyle.cpp
    )

set (part_DIR_SRCS
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

ki18n_wrap_ui(part_DIR_SRCS
    part/dialogs/FileOptionsWidget.ui
    part/dialogs/InterfaceOptionsWidget.ui
    part/dialogs/PageLayoutSheetPage.ui
    part/dialogs/SheetPropertiesWidget.ui
    part/dialogs/SheetSelectWidget.ui
)

set (ui_DIR_SRCS
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

set (calligrasheetscommon_LIB_SRCS
    MapModel.cpp
    PageManager.cpp
    RegionModel.cpp
    tests/inspector.cpp
    ${chart_DIR_SRCS}
    ${commands_DIR_SRCS}
    ${database_DIR_SRCS}
    ${dialogs_DIR_SRCS}
    ${functions_DIR_SRCS}
    ${part_DIR_SRCS}
    ${ui_DIR_SRCS}
    )

if(Qt5DBus_FOUND)
set (calligrasheetscommon_LIB_SRCS
    ${calligrasheetscommon_LIB_SRCS}
    ${interfaces_DIR_SRCS}
    )
endif()

set (calligrasheetsodf_LIB_SRCS
    SheetsDebug.cpp

    part/Digest.cpp
    ApplicationSettings.cpp
    Binding.cpp
    BindingManager.cpp
    BindingModel.cpp
    BindingStorage.cpp
    CalculationSettings.cpp
    Cell.cpp
    CellStorage.cpp
    Cluster.cpp
    Condition.cpp
    ConditionsStorage.cpp
    Currency.cpp
    Damages.cpp
    DependencyManager.cpp
    DocBase.cpp
    Format.cpp
    Formula.cpp
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
    ValidityStorage.cpp
    Value.cpp
    ValueCalc.cpp
    ValueConverter.cpp
    ValueFormatter.cpp
    ValueParser.cpp

    database/Database.cpp
    database/DatabaseManager.cpp
    database/DatabaseStorage.cpp
    database/Filter.cpp

    ${odf_DIR_SRCS}

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


add_library(calligrasheetsodf SHARED ${calligrasheetsodf_LIB_SRCS})
generate_export_header(calligrasheetsodf
    EXPORT_FILE_NAME sheets_odf_generated_export.h
    BASE_NAME CALLIGRA_SHEETS_ODF
)

target_link_libraries(calligrasheetsodf
    PUBLIC
        komain
        KF5::KDELibs4Support
    PRIVATE
        koplugin
)

set_target_properties(calligrasheetsodf PROPERTIES
   VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetsodf ${INSTALL_TARGETS_DEFAULT_ARGS})


add_library(calligrasheetscommon SHARED ${calligrasheetscommon_LIB_SRCS})
generate_export_header(calligrasheetscommon
    EXPORT_FILE_NAME sheets_common_generated_export.h
    BASE_NAME CALLIGRA_SHEETS_COMMON
)

target_link_libraries(calligrasheetscommon
    PUBLIC
        komain
        calligrasheetsodf
    PRIVATE
        koplugin
        KF5::SonnetCore
        KF5::SonnetUi
        KF5::NotifyConfig
        KF5::KCMUtils
)
if(Qt5Sql_FOUND)
    target_link_libraries(calligrasheetscommon PRIVATE Qt5::Sql)
endif()


set_target_properties(calligrasheetscommon PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetscommon  ${INSTALL_TARGETS_DEFAULT_ARGS})
########### next target ###############

set (calligrasheetspart_PART_SRCS part/Factory_init.cpp )

add_library(calligrasheetspart MODULE ${calligrasheetspart_PART_SRCS})

calligra_part_desktop_to_json(calligrasheetspart sheetspart.desktop)

target_link_libraries(calligrasheetspart calligrasheetscommon )

install(TARGETS calligrasheetspart  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/parts)

########### install files ###############

install( FILES  calligrasheets.rc calligrasheets_readonly.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/calligrasheets)
install( FILES  ui/CellToolOptionWidgets.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets)
install( FILES  calligrasheets.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
install( FILES  calligrasheetsrc DESTINATION ${CONFIG_INSTALL_DIR})

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended
# propertiessheets_plugin.desktop, sheets_viewplugin.desktop

if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    sheets_odf_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/sheets_odf_generated_export.h
    sheets_common_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/sheets_common_generated_export.h

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
    odf/OdfLoadingContext.h
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
    part/Part.h
    part/Find.h
    part/HeaderItems.h
    part/Headers.h
    part/ToolRegistry.h
    part/View.h
DESTINATION ${INCLUDE_INSTALL_DIR}/calligrasheets/part COMPONENT Devel)
install( FILES
    ui/CellToolBase.h
    ui/CellEditorBase.h
    ui/Selection.h
    ui/SheetView.h
DESTINATION ${INCLUDE_INSTALL_DIR}/calligrasheets/ui COMPONENT Devel)
install( FILES
    database/Database.h
    database/Filter.h
DESTINATION ${INCLUDE_INSTALL_DIR}/calligrasheets/database COMPONENT Devel)
install( FILES
    commands/AbstractRegionCommand.h
    commands/DataManipulators.h
    commands/SortManipulator.h
    dialogs/pivot.h
    dialogs/pivotfilters.h
    dialogs/pivotoptions.h
    dialogs/pivotmain.h
DESTINATION ${INCLUDE_INSTALL_DIR}/calligrasheets/commands COMPONENT Devel)

endif()

endif ()

########### APP ###############

if (SHOULD_BUILD_APP_SHEETS)

set (calligrasheets_KDEINIT_SRCS part/Main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/data/pics/*-apps-calligrasheets.png")
ecm_add_app_icon(kdeinit_app_ICONS_SRCS ICONS ${ICONS_SRCS})
if(WIN32)
    set(_resourcefile "${CMAKE_CURRENT_BINARY_DIR}/kdeinit_app_ICONS_SRCS.rc")
endif()

kf5_add_kdeinit_executable( calligrasheets ${calligrasheets_KDEINIT_SRCS})

if (APPLE)
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.sheets")
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Calligra Sheets 2")
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/calligrasheets_KDEINIT_SRCS.icns DESTINATION ${BUNDLE_INSTALL_DIR}/calligrasheets.app/Contents/Resources)
endif ()

target_link_libraries(kdeinit_calligrasheets komain KF5::KDELibs4Support)

install(TARGETS kdeinit_calligrasheets  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligrasheets kdeinit_calligrasheets )
install(TARGETS calligrasheets  ${INSTALL_TARGETS_DEFAULT_ARGS})

install( PROGRAMS  org.kde.calligrasheets.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES org.kde.calligrasheets.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

endif ()
