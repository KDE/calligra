
set (actions_DIR_SRCS
    actions/CellAction.cpp
    actions/DialogCellAction.cpp
    actions/Actions.cpp
    actions/AdjustRowCol.cpp
    actions/Align.cpp
    actions/Angle.cpp
    actions/AutoFilter.cpp
    actions/AutoFormat.cpp
    actions/AutoSum.cpp
    actions/Border.cpp
    actions/Cells.cpp
    actions/Clear.cpp
    actions/Comment.cpp
    actions/Conditional.cpp
    actions/Consolidate.cpp
    actions/Copy.cpp
    actions/CSVActions.cpp
    actions/CustomLists.cpp
    actions/DocumentSettings.cpp
    actions/Editing.cpp
    actions/Fill.cpp
    actions/FindReplace.cpp
    actions/Font.cpp
    actions/Formula.cpp
    actions/GoalSeek.cpp
    actions/Goto.cpp
    actions/Indent.cpp
    actions/InsertFromDatabase.cpp
    actions/InsertSeries.cpp
    actions/InsertSpecialChar.cpp
    actions/Inspector.cpp
    actions/Link.cpp
    actions/ListChoose.cpp
    actions/Merge.cpp
    actions/NamedAreas.cpp
    actions/PageBreak.cpp
    actions/Paste.cpp
#    actions/Pivot.cpp
    actions/SelectAll.cpp
    actions/ShowTableView.cpp
    actions/Sort.cpp
    actions/SpellCheck.cpp
    actions/Style.cpp
    actions/Styles.cpp
    actions/Subtotals.cpp
    actions/TextCase.cpp
    actions/Validity.cpp
    )

set (action_dialogs_DIR_SRCS
    actions/dialogs/ActionDialog.cpp
    actions/dialogs/AddNamedAreaDialog.cpp
    actions/dialogs/AngleDialog.cpp
    actions/dialogs/AutoFormatDialog.cpp
    actions/dialogs/CharacterSelectDialog.cpp
    actions/dialogs/CommentDialog.cpp
    actions/dialogs/ConditionalDialog.cpp
    actions/dialogs/ConsolidateDialog.cpp
    actions/dialogs/CSVDialog.cpp
    actions/dialogs/DatabaseDialog.cpp
    actions/dialogs/DocumentSettingsDialog.cpp
    actions/dialogs/FindDialog.cpp
    actions/dialogs/FormulaDialog.cpp
    actions/dialogs/GoalSeekDialog.cpp
    actions/dialogs/GotoDialog.cpp
    actions/dialogs/InsertDialog.cpp
    actions/dialogs/LinkDialog.cpp
    actions/dialogs/ListDialog.cpp
    actions/dialogs/NamedAreaDialog.cpp
    actions/dialogs/PasteInsertDialog.cpp
    actions/dialogs/Resize2Dialog.cpp
    actions/dialogs/SeriesDialog.cpp
    actions/dialogs/ShowColRowDialog.cpp
    actions/dialogs/SortDialog.cpp
    actions/dialogs/SpecialPasteDialog.cpp
    actions/dialogs/StyleManagerDialog.cpp
    actions/dialogs/SubtotalDialog.cpp
    actions/dialogs/ValidityDialog.cpp
#    actions/dialogs/pivot.cpp
#    actions/dialogs/pivotfilters.cpp
#    actions/dialogs/pivotoptions.cpp
#    actions/dialogs/pivotmain.cpp
    )

ki18n_wrap_ui(action_dialogs_DIR_SRCS
    actions/dialogs/ConsolidateWidget.ui
    actions/dialogs/ConsolidateDetailsWidget.ui
    actions/dialogs/GoalSeekWidget.ui
    actions/dialogs/SortWidget.ui
    actions/dialogs/SortDetailsWidget.ui
    actions/dialogs/SpecialPasteWidget.ui
    actions/dialogs/SubtotalWidget.ui
    actions/dialogs/SubtotalsDetailsWidget.ui
#    actions/dialogs/pivot.ui
#    actions/dialogs/pivotfilters.ui
#    actions/dialogs/pivotoptions.ui
#    actions/dialogs/pivotmain.ui
    )

set (commands_DIR_SRCS
    commands/AbstractRegionCommand.cpp
    commands/DataManipulators.cpp
    commands/ApplyFilterCommand.cpp
    commands/AutoFillCommand.cpp
    commands/MergeCommand.cpp
    commands/NamedAreaCommand.cpp
    commands/PageBreakCommand.cpp
    commands/RowColumnManipulators.cpp
    commands/SheetCommands.cpp
    commands/StyleCommand.cpp
    commands/CopyCommand.cpp
    commands/PasteCommand.cpp
    )


set (dialogs_DIR_SRCS
    dialogs/LayoutWidgets.cpp
    dialogs/LayoutPage_Border.cpp
    dialogs/LayoutPage_Float.cpp
    dialogs/LayoutPage_Font.cpp
    dialogs/LayoutPage_General.cpp
    dialogs/LayoutPage_Pattern.cpp
    dialogs/LayoutPage_Position.cpp
    dialogs/LayoutPage_Protection.cpp
    dialogs/LayoutDialog.cpp
    dialogs/ShowDialog.cpp
    )


ki18n_wrap_ui(dialogs_DIR_SRCS
    dialogs/FontWidget.ui
    dialogs/PositionWidget.ui
    dialogs/ProtectionWidget.ui
    )

set (strategy_DIR_SRCS
    strategy/AutoFillStrategy.cpp
    strategy/DragAndDropStrategy.cpp
    strategy/HyperlinkStrategy.cpp
    strategy/MergeStrategy.cpp
    strategy/PasteStrategy.cpp
    strategy/SelectionStrategy.cpp
    )

set (calligrasheetsui_LIB_SRCS
    AbstractSelectionStrategy.cpp
    ActionOptionWidget.cpp
    CellEditorBase.cpp
    CellEditor.cpp
    CellToolBase.cpp
    CellToolBase_p.cpp
    CellView.cpp
    ExternalEditor.cpp
    FilterPopup.cpp
    FormulaEditorHighlighter.cpp
    FunctionCompletion.cpp
    LocationComboBox.cpp
    MapModel.cpp
    MapViewModel.cpp
    RegionSelector.cpp
    Selection.cpp
    SheetView.cpp

    ${actions_DIR_SRCS}
    ${action_dialogs_DIR_SRCS}
    ${commands_DIR_SRCS}
    ${dialogs_DIR_SRCS}
    ${strategy_DIR_SRCS}
)

if(NOT Qt6Sql_FOUND)
    add_definitions(-DQT_NO_SQL)
endif()

add_library(calligrasheetsui SHARED ${calligrasheetsui_LIB_SRCS})

target_include_directories( calligrasheetsui
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)

generate_export_header(calligrasheetsui
    EXPORT_FILE_NAME sheets_ui_generated_export.h
    BASE_NAME CALLIGRA_SHEETS_UI
)
if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    sheets_ui_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/sheets_ui_generated_export.h

    CellToolBase.h
    CellEditorBase.h
    Selection.h
    SheetView.h

    commands/AbstractRegionCommand.h
    commands/DataManipulators.h

DESTINATION ${KDE_INSTALL_INCLUDEDIR}/sheets/ui COMPONENT Devel)

endif()

target_link_libraries(calligrasheetsui
    PUBLIC
        calligrasheetsengine
        calligrasheetscore
        komain
        KF6::GuiAddons
        KF6::Notifications
        KF6::SonnetCore

)
if(Qt6Sql_FOUND)
    target_link_libraries(calligrasheetsui PRIVATE Qt6::Sql)
endif()

set_target_properties(calligrasheetsui PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS calligrasheetsui ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install( FILES  CellToolOptionWidgets.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets)

