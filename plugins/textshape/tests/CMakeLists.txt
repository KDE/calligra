set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/plugins/textshape 
    ${CMAKE_SOURCE_DIR}/plugins/textshape/dialogs
    ${CMAKE_BINARY_DIR}/plugins/textshape/dialogs
    ${CMAKE_BINARY_DIR}/plugins/textshape
    ${KOTEXT_INCLUDES}
    ${PIGMENT_INCLUDES})

# call: textshape_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(TEXTSHAPE_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "shapes-text-"
    )
endmacro()


########### next target ###############

set(TestDocumentLayout_test_SRCS
    TestDocumentLayout.cpp
    TestLists.cpp
    ../TextShapeLayout.cpp
    ../TextLine.cpp
    ../Outline.cpp
    ../TextShape.cpp
    ../ListItemsHelper.cpp
    ../TableLayout.cpp
    ../TableLayoutData.cpp
    ../ToCGenerator.cpp
    ../ShrinkToFitShapeContainer.cpp
    ../FloatingAnchorStrategy.cpp
    ../InlineAnchorStrategy.cpp
)

textshape_add_unit_test(TestDocumentLayout
    ${TestDocumentLayout_test_SRCS}
    LINK_LIBRARIES Qt6::Test
)

########### next target ###############

set(TestChangeListCommand_test_SRCS
    TestChangeListCommand.cpp
    ../commands/TextCommandBase.cpp
    ../commands/TextCutCommand.cpp
    ../commands/ShowChangesCommand.cpp
    ../commands/AcceptChangeCommand.cpp
    ../commands/RejectChangeCommand.cpp
    ../commands/AutoResizeCommand.cpp
    ../TextTool.cpp
    ../TextEditingPluginContainer.cpp
    ../ChangeTracker.cpp
    ../dialogs/FontDia.cpp
    ../dialogs/InsertCharacter.cpp
    ../dialogs/StyleManagerDialog.cpp
    ../dialogs/SimpleStylesWidget.cpp
    ../dialogs/SimpleCharacterWidget.cpp
    ../dialogs/ParagraphSettingsDialog.cpp
    ../dialogs/SimpleParagraphWidget.cpp
    ../dialogs/FormattingButton.cpp
    ../dialogs/SimpleTableWidget.cpp
    ../dialogs/QuickTableButton.cpp
    ../dialogs/StylesWidget.cpp
    ../dialogs/FontTab.cpp
    ../dialogs/CharacterHighlighting.cpp
    ../dialogs/FontDecorations.cpp
    ../dialogs/FontLayoutTab.cpp
    ../dialogs/LanguageTab.cpp
    ../dialogs/StyleManager.cpp
    ../dialogs/ParagraphIndentSpacing.cpp
    ../dialogs/ParagraphLayout.cpp
    ../dialogs/ParagraphDecorations.cpp
    ../dialogs/ParagraphBulletsNumbers.cpp
    ../dialogs/ListsSpinBox.cpp
    ../dialogs/StylesModel.cpp
    ../dialogs/StylesDelegate.cpp
    ../dialogs/ParagraphGeneral.cpp
    ../dialogs/CharacterGeneral.cpp
    ../dialogs/NewStyleWidget.cpp
    ../dialogs/FormattingPreview.cpp
    ../dialogs/StyleManagerWelcome.cpp
    ../dialogs/TableDialog.cpp
    ../dialogs/TrackedChangeManager.cpp
    ../dialogs/TrackedChangeModel.cpp
    ../dialogs/ChangeConfigureDialog.cpp
    ../dialogs/AcceptRejectChangeDialog.cpp
    ../dialogs/ChangeTrackingOptionsWidget.cpp
    ../dialogs/SimpleTableWidget.cpp
    ../ListItemsHelper.cpp
    ../TextShape.cpp
    ../TextShapeLayout.cpp
    ../TextLine.cpp
    ../Outline.cpp
    ../TableLayout.cpp
    ../TableLayoutData.cpp
    ../ToCGenerator.cpp
    ../ShrinkToFitShapeContainer.cpp
    ../FontSizeAction.cpp
    ../FloatingAnchorStrategy.cpp
    ../InlineAnchorStrategy.cpp
)

textshape_add_unit_test(TestChangeListCommand
    ${TestChangeListCommand_test_SRCS}
    LINK_LIBRARIES Qt6::Test
)

########### next target ###############

set(TestChangesDatabase_test_SRCS
    TestChangesDatabase.cpp
    ../TextChanges.cpp
    ../TextChange.cpp
)
textshape_add_unit_test(TestChangesDatabase
    ${TestChangesDatabase_test_SRCS}
    LINK_LIBRARIES Qt6::Test
)

########### next target ###############

#set(TestStylesModel_test_SRCS
#    TestStylesModel.cpp
#    ../dialogs/StylesModel.cpp
#    ../TextShape.cpp
#    ../Layout.cpp
#    ../TextLine.cpp
#    ../Outline.cpp
#    ../TableLayout.cpp
#    ../TableLayoutData.cpp
#    ../ListItemsHelper.cpp
#    ../ToCGenerator.cpp
#    ../ShrinkToFitShapeContainer.cpp
#    ../FloatingAnchorStrategy.cpp
#    ../InlineAnchorStrategy.cpp
#)
#textshape_add_unit_test(TestStylesModel
#    ${TestStylesModel_test_SRCS}
#    LINK_LIBRARIES kotext Qt6::Test
#)

########### next target ###############

set(TestTableLayout_test_SRCS
    TestTableLayout.cpp
    ../TableLayout.cpp
    ../TableLayoutData.cpp
    ../TextShapeLayout.cpp
    ../TextLine.cpp
    ../Outline.cpp
    ../TextShape.cpp
    ../ListItemsHelper.cpp
    ../ToCGenerator.cpp
    ../ShrinkToFitShapeContainer.cpp
    ../FloatingAnchorStrategy.cpp
    ../InlineAnchorStrategy.cpp
)
textshape_add_unit_test(TestTableLayout
    ${TestTableLayout_test_SRCS}
    LINK_LIBRARIES kotext Qt6::Test
)

########### next target ###############

set(TestSections_test_SRCS
    TestSections.cpp
    ../TableLayout.cpp
    ../TableLayoutData.cpp
    ../TextShapeLayout.cpp
    ../TextShape.cpp
    ../ListItemsHelper.cpp
    ../ToCGenerator.cpp
    ../ShrinkToFitShapeContainer.cpp
    ../TextLine.cpp
    ../Outline.cpp
    ../FloatingAnchorStrategy.cpp
    ../InlineAnchorStrategy.cpp
)
textshape_add_unit_test(TestSections
    ${TestSections_test_SRCS}
    LINK_LIBRARIES kotext Qt6::Test
)

########### next target ###############

set(TestTextTool_test_SRCS
    TestTextTool.cpp
    ../ChangeTracker.cpp
    ../commands/AcceptChangeCommand.cpp
    ../commands/ChangeListCommand.cpp
    ../commands/ChangeListLevelCommand.cpp
    ../commands/ChangeTrackedDeleteCommand.cpp
    ../commands/DeleteCommand.cpp
    ../commands/ListItemNumberingCommand.cpp
    ../commands/RejectChangeCommand.cpp
    ../commands/ShowChangesCommand.cpp
    ../commands/TextCommandBase.cpp
    ../commands/TextCutCommand.cpp
    ../commands/TextPasteCommand.cpp
    ../commands/AutoResizeCommand.cpp
    ../dialogs/ChangeConfigureDialog.cpp
    ../dialogs/AcceptRejectChangeDialog.cpp
    ../dialogs/CharacterGeneral.cpp
    ../dialogs/CharacterHighlighting.cpp
    ../dialogs/FontDecorations.cpp
    ../dialogs/FontDia.cpp
    ../dialogs/FontLayoutTab.cpp
    ../dialogs/FontTab.cpp
    ../dialogs/FormattingPreview.cpp
    ../dialogs/InsertCharacter.cpp
    ../dialogs/LanguageTab.cpp
    ../dialogs/ListsSpinBox.cpp
    ../dialogs/NewStyleWidget.cpp
    ../dialogs/ParagraphBulletsNumbers.cpp
    ../dialogs/ParagraphDecorations.cpp
    ../dialogs/ParagraphGeneral.cpp
    ../dialogs/ParagraphIndentSpacing.cpp
    ../dialogs/ParagraphLayout.cpp
    ../dialogs/ParagraphSettingsDialog.cpp
    ../dialogs/StyleManager.cpp
    ../dialogs/StyleManagerDialog.cpp
    ../dialogs/StyleManagerWelcome.cpp
    ../dialogs/StylesModel.cpp
    ../dialogs/StylesDelegate.cpp
    ../dialogs/SimpleStylesWidget.cpp
    ../dialogs/SimpleCharacterWidget.cpp
    ../dialogs/SimpleParagraphWidget.cpp
    ../dialogs/FormattingButton.cpp
    ../dialogs/SimpleTableWidget.cpp
    ../dialogs/QuickTableButton.cpp
    ../dialogs/StylesWidget.cpp
    ../dialogs/TableDialog.cpp
    ../dialogs/TrackedChangeManager.cpp
    ../dialogs/TrackedChangeModel.cpp
    ../dialogs/ChangeTrackingOptionsWidget.cpp
    ../TextShapeLayout.cpp
    ../TextLine.cpp
    ../Outline.cpp
    ../ListItemsHelper.cpp
    ../TableLayout.cpp
    ../TableLayoutData.cpp
    ../TextEditingPluginContainer.cpp
    ../TextShape.cpp
    ../TextTool.cpp
    ../ToCGenerator.cpp
    ../ShrinkToFitShapeContainer.cpp
    ../FontSizeAction.cpp
    ../FloatingAnchorStrategy.cpp
    ../InlineAnchorStrategy.cpp
)
textshape_add_unit_test(TestTextTool
    ${TestTextTool_test_SRCS}
    LINK_LIBRARIES Qt6::Test
)

########### next target ###############

set(TestChangeTrackedDelete_test_SRCS
    TestChangeTrackedDelete.cpp
    ../commands/ChangeListCommand.cpp
    ../commands/ChangeListLevelCommand.cpp
    ../commands/ListItemNumberingCommand.cpp
    ../commands/TextCommandBase.cpp
    ../commands/TextPasteCommand.cpp
    ../commands/TextCutCommand.cpp
    ../commands/ChangeTrackedDeleteCommand.cpp
    ../commands/DeleteCommand.cpp
    ../commands/ShowChangesCommand.cpp
    ../commands/AcceptChangeCommand.cpp
    ../commands/RejectChangeCommand.cpp
    ../commands/AutoResizeCommand.cpp
    ../TextTool.cpp
    ../TextEditingPluginContainer.cpp
    ../ChangeTracker.cpp
    ../dialogs/FontDia.cpp
    ../dialogs/InsertCharacter.cpp
    ../dialogs/StyleManagerDialog.cpp
    ../dialogs/ParagraphSettingsDialog.cpp
    ../dialogs/SimpleStylesWidget.cpp
    ../dialogs/SimpleCharacterWidget.cpp
    ../dialogs/SimpleParagraphWidget.cpp
    ../dialogs/FormattingButton.cpp
    ../dialogs/SimpleTableWidget.cpp
    ../dialogs/QuickTableButton.cpp
    ../dialogs/StylesWidget.cpp
    ../dialogs/FontTab.cpp
    ../dialogs/CharacterHighlighting.cpp
    ../dialogs/FontDecorations.cpp
    ../dialogs/FontLayoutTab.cpp
    ../dialogs/LanguageTab.cpp
    ../dialogs/StyleManager.cpp
    ../dialogs/ParagraphIndentSpacing.cpp
    ../dialogs/ParagraphLayout.cpp
    ../dialogs/ParagraphDecorations.cpp
    ../dialogs/ParagraphBulletsNumbers.cpp
    ../dialogs/ListsSpinBox.cpp
    ../dialogs/StylesModel.cpp
    ../dialogs/StylesDelegate.cpp
    ../dialogs/ParagraphGeneral.cpp
    ../dialogs/CharacterGeneral.cpp
    ../dialogs/NewStyleWidget.cpp
    ../dialogs/FormattingPreview.cpp
    ../dialogs/StyleManagerWelcome.cpp
    ../dialogs/TableDialog.cpp
    ../dialogs/TrackedChangeManager.cpp
    ../dialogs/TrackedChangeModel.cpp
    ../dialogs/ChangeConfigureDialog.cpp
    ../dialogs/AcceptRejectChangeDialog.cpp
    ../dialogs/ChangeTrackingOptionsWidget.cpp
    ../dialogs/SimpleTableWidget.cpp
    ../ListItemsHelper.cpp
    ../TextShape.cpp
    ../TextShapeLayout.cpp
    ../TextLine.cpp
    ../Outline.cpp
    ../TableLayout.cpp
    ../TableLayoutData.cpp
    ../ToCGenerator.cpp
    ../ShrinkToFitShapeContainer.cpp
    ../FontSizeAction.cpp
    ../FloatingAnchorStrategy.cpp
    ../InlineAnchorStrategy.cpp
)
textshape_add_unit_test(TestChangeTrackedDelete
    ${TestChangeTrackedDelete_test_SRCS}
    LINK_LIBRARIES kotext Qt6::Test
)

