project( textPlugin)

add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_text\")

# only build the textdocument inspector in non-release builds
if(NOT RELEASE_BUILD)
    set(BUILD_TEXTDOCUMENT_INSPECTOR TRUE)
    add_definitions(-DCREATE_TEXTDOCUMENT_INSPECTOR)
else()
    set(BUILD_TEXTDOCUMENT_INSPECTOR FALSE)
endif()


add_subdirectory( pics )
#add_subdirectory( tests )

include_directories(
    ${KOTEXT_INCLUDES}
    ${TEXTLAYOUT_INCLUDES}
    ${KOWIDGETS_INCLUDES}
)

########### Flake Plugin library ###############

set ( textshape_SRCS
    TextShapeDebug.cpp
    TextPlugin.cpp
    TextShape.cpp
    TextShapeFactory.cpp
    TextTool.cpp
    TextEditingPluginContainer.cpp
    TextToolFactory.cpp
    ShrinkToFitShapeContainer.cpp
    SimpleRootAreaProvider.cpp
    AnnotationTextShape.cpp
    AnnotationTextShapeFactory.cpp

    ChangeTracker.cpp
    ReviewTool.cpp
    ReviewToolFactory.cpp
    TextChanges.cpp
    TextChange.cpp
    FontSizeAction.cpp
    FontFamilyAction.cpp

    ReferencesTool.cpp
    ReferencesToolFactory.cpp

#    dialogs/StylesWidget.cpp
#    dialogs/SpecialButton.cpp
    dialogs/StylesCombo.cpp
    dialogs/StylesComboPreview.cpp
    dialogs/DockerStylesComboModel.cpp
    dialogs/SimpleCharacterWidget.cpp
    dialogs/SimpleParagraphWidget.cpp
    dialogs/SimpleTableWidget.cpp
    dialogs/SimpleInsertWidget.cpp
    dialogs/LinkInsertionDialog.cpp
    dialogs/SimpleTableOfContentsWidget.cpp
    dialogs/SimpleCitationBibliographyWidget.cpp
    dialogs/SimpleLinksWidget.cpp
    dialogs/SimpleSpellCheckingWidget.cpp
    dialogs/CitationInsertionDialog.cpp
    dialogs/InsertBibliographyDialog.cpp
    dialogs/SimpleFootEndNotesWidget.cpp
    dialogs/NotesConfigurationDialog.cpp
    dialogs/SimpleCaptionsWidget.cpp
    dialogs/ParagraphLayout.cpp
    dialogs/ParagraphIndentSpacing.cpp
    dialogs/ParagraphDecorations.cpp
    dialogs/ParagraphBulletsNumbers.cpp
    dialogs/ParagraphSettingsDialog.cpp
    dialogs/ParagraphDropCaps.cpp
    dialogs/ListsSpinBox.cpp
    dialogs/StylesModel.cpp
    dialogs/StylesManagerModel.cpp
    dialogs/StylesSortFilterProxyModel.cpp
    dialogs/AbstractStylesModel.cpp
    dialogs/StylesFilteredModelBase.cpp
    dialogs/ValidParentStylesProxyModel.cpp
    dialogs/StylesDelegate.cpp
    dialogs/StyleManager.cpp
    dialogs/StyleManagerDialog.cpp
    dialogs/ParagraphGeneral.cpp
    dialogs/CharacterGeneral.cpp
    dialogs/CharacterHighlighting.cpp
    dialogs/InsertCharacter.cpp
    dialogs/FontDia.cpp
    dialogs/FontDecorations.cpp
    dialogs/LanguageTab.cpp
    dialogs/FormattingPreview.cpp
    dialogs/StyleManagerWelcome.cpp
    dialogs/TableDialog.cpp
    dialogs/QuickTableButton.cpp
    dialogs/FormattingButton.cpp
    dialogs/ChangeConfigureDialog.cpp
    dialogs/AcceptRejectChangeDialog.cpp
    dialogs/TrackedChangeModel.cpp
    dialogs/TrackedChangeManager.cpp
    dialogs/BibliographyConfigureDialog.cpp
    dialogs/TableOfContentsConfigure.cpp
    dialogs/TableOfContentsStyleConfigure.cpp
    dialogs/TableOfContentsStyleModel.cpp
    dialogs/TableOfContentsStyleDelegate.cpp
    dialogs/TableOfContentsPreview.cpp
    dialogs/TableOfContentsEntryDelegate.cpp
    dialogs/TableOfContentsEntryModel.cpp
    dialogs/TableOfContentsTemplate.cpp
    dialogs/BibliographyTemplate.cpp
    dialogs/BibliographyPreview.cpp
    dialogs/ListLevelWidget.cpp
    dialogs/ListLevelChooser.cpp
    dialogs/SimpleAnnotationWidget.cpp
    dialogs/ManageBookmarkDialog.cpp
    dialogs/SectionFormatDialog.cpp
    dialogs/SectionsSplitDialog.cpp

    commands/ChangeListLevelCommand.cpp
    commands/ShowChangesCommand.cpp
    commands/AcceptChangeCommand.cpp
    commands/RejectChangeCommand.cpp
    commands/AutoResizeCommand.cpp
)

if(BUILD_TEXTDOCUMENT_INSPECTOR)
    set ( textshape_SRCS
        ${textshape_SRCS}
        TextDocumentStructureModel.cpp
        TextDocumentInspectionDocker.cpp
        TextDocumentInspectionDockerFactory.cpp
    )
endif()


ki18n_wrap_ui(textshape_SRCS
    dialogs/SimpleCharacterWidget.ui
    dialogs/SimpleParagraphWidget.ui
    dialogs/SimpleTableWidget.ui
    dialogs/SimpleInsertWidget.ui
    dialogs/SimpleTableOfContentsWidget.ui
    dialogs/SimpleCitationBibliographyWidget.ui
    dialogs/SimpleSpellCheckingWidget.ui
    dialogs/CitationInsertionDialog.ui
    dialogs/InsertBibliographyDialog.ui
    dialogs/SimpleFootEndNotesWidget.ui
    dialogs/NotesConfigurationDialog.ui
    dialogs/SimpleCaptionsWidget.ui
    dialogs/StylesWidget.ui
    dialogs/ParagraphLayout.ui
    dialogs/ParagraphIndentSpacing.ui
    dialogs/ParagraphDecorations.ui
    dialogs/ParagraphBulletsNumbers.ui
    dialogs/ParagraphDropCaps.ui
    dialogs/StyleManager.ui
    dialogs/CharacterGeneral.ui
    dialogs/CharacterHighlighting.ui
    dialogs/StyleManagerWelcome.ui
    dialogs/TableDialog.ui
    dialogs/BibliographyConfigureDialog.ui
    dialogs/TableOfContentsConfigure.ui
    dialogs/SimpleLinksWidget.ui
    dialogs/TableOfContentsStyleConfigure.ui
    dialogs/SimpleAnnotationWidget.ui


    dialogs/ListLevelWidget.ui
    dialogs/FontDecorations.ui
    dialogs/LanguageTab.ui
    dialogs/ChangeConfigureDialog.ui
    dialogs/AcceptRejectChangeDialog.ui
    dialogs/TrackedChangeManager.ui
    dialogs/LinkInsertionDialog.ui
    dialogs/ManageBookmark.ui
    dialogs/SectionFormatDialog.ui
    dialogs/SectionsSplitDialog.ui

)

add_library(calligra_shape_text MODULE ${textshape_SRCS})


target_link_libraries(calligra_shape_text
  kotext
  kotextlayout
  kowidgets
  KF6::KIOCore
  KF6::KIOFileWidgets
  KF6::KIOWidgets
  KF6::ItemViews
  KF6::WidgetsAddons
  KF6::Completion
  Qt6::Network
)

if( SHOULD_BUILD_FEATURE_RDF )
    target_link_libraries(calligra_shape_text ${SOPRANO_LIBRARIES})
endif()

install(TARGETS calligra_shape_text DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)
