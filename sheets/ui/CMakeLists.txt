
set (actions_DIR_SRCS
    actions/CellAction.cpp
    actions/Actions.cpp
    actions/AdjustRowCol.cpp
    actions/AutoSum.cpp
    actions/Comment.cpp
    actions/CSVActions.cpp
    actions/Fill.cpp
    actions/Indent.cpp
    actions/InsertSeries.cpp
    actions/InsertSpecialChar.cpp
    actions/Link.cpp
    actions/Merge.cpp
    actions/Sort.cpp
    actions/TextCase.cpp
    actions/Validity.cpp
    )

set (action_dialogs_DIR_SRCS
    actions/dialogs/CharacterSelectDialog.cpp
    actions/dialogs/CommentDialog.cpp
    actions/dialogs/CSVDialog.cpp
    actions/dialogs/LinkDialog.cpp
    actions/dialogs/Resize2Dialog.cpp
    actions/dialogs/SeriesDialog.cpp
    actions/dialogs/ShowColRowDialog.cpp
    actions/dialogs/SortDialog.cpp
    actions/dialogs/ValidityDialog.cpp
    )

ki18n_wrap_ui(action_dialogs_DIR_SRCS
    actions/dialogs/SortWidget.ui
    actions/dialogs/SortDetailsWidget.ui
    )

set (commands_DIR_SRCS
    commands/AbstractRegionCommand.cpp
    commands/DataManipulators.cpp
    commands/ApplyFilterCommand.cpp
    commands/AutoFillCommand.cpp
    commands/AutoFilterCommand.cpp
    commands/AutoFormatCommand.cpp
    commands/BorderColorCommand.cpp
    commands/ConditionCommand.cpp
    commands/CSVDataCommand.cpp
    commands/DeleteCommand.cpp
    commands/MergeCommand.cpp
    commands/NamedAreaCommand.cpp
    commands/PageBreakCommand.cpp
    commands/PrecisionCommand.cpp
    commands/RowColumnManipulators.cpp
    commands/SheetCommands.cpp
    commands/SpellCheckCommand.cpp
    commands/StyleCommand.cpp
    commands/CopyCommand.cpp
    commands/PasteCommand.cpp
    )


set (dialogs_DIR_SRCS
    dialogs/AddNamedAreaDialog.cpp
    dialogs/AngleDialog.cpp
    dialogs/AutoFormatDialog.cpp
    dialogs/ConditionalDialog.cpp
    dialogs/ConsolidateDialog.cpp
    dialogs/DatabaseDialog.cpp
    dialogs/DocumentSettingsDialog.cpp
    dialogs/FindDialog.cpp
    dialogs/FormulaDialog.cpp
    dialogs/GoalSeekDialog.cpp
    dialogs/GotoDialog.cpp
    dialogs/InsertDialog.cpp
    dialogs/LayoutDialog.cpp
    dialogs/ListDialog.cpp
    dialogs/NamedAreaDialog.cpp
    dialogs/PasteInsertDialog.cpp
    dialogs/ShowDialog.cpp
    dialogs/SpecialPasteDialog.cpp
    dialogs/StyleManagerDialog.cpp
    dialogs/SubtotalDialog.cpp
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
    dialogs/SubtotalWidget.ui
    dialogs/SubtotalsDetailsWidget.ui
    dialogs/pivot.ui
    dialogs/pivotfilters.ui
    dialogs/pivotoptions.ui
    dialogs/pivotmain.ui

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
    inspector.cpp
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

if(NOT Qt5Sql_FOUND)
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

    dialogs/pivot.h
    dialogs/pivotfilters.h
    dialogs/pivotoptions.h
    dialogs/pivotmain.h

DESTINATION ${KDE_INSTALL_INCLUDEDIR}/sheets/ui COMPONENT Devel)

endif()

target_link_libraries(calligrasheetsui
    PUBLIC
        calligrasheetsengine
        calligrasheetscore
        komain
        KF5::GuiAddons
        KF5::Notifications
        KF5::SonnetCore

)
if(Qt5Sql_FOUND)
    target_link_libraries(calligrasheetsui PRIVATE Qt5::Sql)
endif()

set_target_properties(calligrasheetsui PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetsui ${INSTALL_TARGETS_DEFAULT_ARGS})

install( FILES  CellToolOptionWidgets.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets)

