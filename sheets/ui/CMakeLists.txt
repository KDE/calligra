
set (commands_DIR_SRCS
    commands/AbstractRegionCommand.cpp
    commands/DataManipulators.cpp
    commands/ApplyFilterCommand.cpp
    commands/AutoFillCommand.cpp
    commands/AutoFilterCommand.cpp
    commands/AutoFormatCommand.cpp
    commands/BorderColorCommand.cpp
    commands/CommentCommand.cpp
    commands/ConditionCommand.cpp
    commands/CSVDataCommand.cpp
    commands/DeleteCommand.cpp
    commands/IndentationCommand.cpp
    commands/LinkCommand.cpp
    commands/MergeCommand.cpp
    commands/NamedAreaCommand.cpp
    commands/PageBreakCommand.cpp
    commands/PrecisionCommand.cpp
    commands/RowColumnManipulators.cpp
    commands/SheetCommands.cpp
    commands/SortManipulator.cpp
    commands/SpellCheckCommand.cpp
    commands/StyleCommand.cpp
    commands/ValidityCommand.cpp
    commands/CopyCommand.cpp
    commands/PasteCommand.cpp
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
    MapViewModel.cpp
    RegionSelector.cpp
    Selection.cpp
    SheetView.cpp

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
    commands/SortManipulator.h

    dialogs/pivot.h
    dialogs/pivotfilters.h
    dialogs/pivotoptions.h
    dialogs/pivotmain.h

DESTINATION ${INCLUDE_INSTALL_DIR}/sheets/ui COMPONENT Devel)

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

install( FILES  CellToolOptionWidgets.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets)

