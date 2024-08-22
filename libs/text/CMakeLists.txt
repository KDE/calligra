include_directories(${KOTEXT_INCLUDES}
                    ${FONTCONFIG_INCLUDE_DIR}
                    ${FREETYPE_INCLUDE_DIRS})

if(BUILD_TESTING)
    add_subdirectory( tests )
    add_subdirectory( styles/tests )
endif()

########### next target ###############
set(kotext_LIB_SRCS
    KoDocumentRdfBase.cpp
    KoText.cpp
    KoTextBlockData.cpp
    KoTextBlockBorderData.cpp
    KoTextBlockPaintStrategyBase.cpp
    KoTextOdfSaveHelper.cpp
    KoTextPaste.cpp
    KoTextDocument.cpp
    KoTextEditor.cpp
    KoTextEditor_undo.cpp
    KoTextEditor_format.cpp
    KoList.cpp

    KoTextEditingRegistry.cpp
    KoTextEditingFactory.cpp
    KoTextEditingPlugin.cpp

    KoTextRangeManager.cpp
    KoInlineTextObjectManager.cpp
    KoInlineObjectFactoryBase.cpp
    KoInlineObjectRegistry.cpp
    InsertInlineObjectActionBase_p.cpp
    InsertVariableAction.cpp
    InsertNamedVariableAction.cpp
    InsertTextReferenceAction.cpp
    InsertTextLocator.cpp
    KoInlineObject.cpp
    KoTextRange.cpp
    KoVariable.cpp
    KoVariableManager.cpp
    KoNamedVariable.cpp
    KoSection.cpp
    KoSectionEnd.cpp
    KoSectionUtils.cpp
    KoSectionModel.cpp
    KoTextLocator.cpp
    KoTextReference.cpp
    KoAnchorInlineObject.cpp
    KoAnchorTextRange.cpp
    KoTextShapeSavingContext.cpp
    KoAnnotation.cpp
    KoAnnotationManager.cpp
    KoBookmark.cpp
    KoBookmarkManager.cpp
    KoInlineNote.cpp
    KoInlineCite.cpp
    KoTextSoftPageBreak.cpp
    FindDirection_p.cpp
    KoFindStrategy.cpp
    KoReplaceStrategy.cpp
    KoFind_p.cpp
    KoFind.cpp
    KoTextDebug.cpp
    KoTextPage.cpp
    KoPageProvider.cpp
    KoTableColumnAndRowStyleManager.cpp
    KoTextInlineRdf.cpp
    KoTextMeta.cpp
    KoTextTableTemplate.cpp

    OdfTextTrackStyles.cpp

    ToCBibGeneratorInfo.cpp
    KoTableOfContentsGeneratorInfo.cpp
    KoBibliographyInfo.cpp
    BibliographyGenerator.cpp

    styles/Styles_p.cpp
    styles/KoCharacterStyle.cpp
    styles/KoParagraphStyle.cpp
    styles/KoStyleManager.cpp
    styles/KoListStyle.cpp
    styles/KoListLevelProperties.cpp
    styles/KoTableStyle.cpp
    styles/KoTableColumnStyle.cpp
    styles/KoTableRowStyle.cpp
    styles/KoTableCellStyle.cpp
    styles/KoSectionStyle.cpp
    opendocument/KoTextSharedLoadingData.cpp
    opendocument/KoTextSharedSavingData.cpp
    opendocument/KoTextLoader.cpp
    opendocument/KoTextWriter_p.cpp
    opendocument/KoTextWriter.cpp

    changetracker/KoChangeTracker.cpp
    changetracker/KoChangeTrackerElement.cpp
    changetracker/KoFormatChangeInformation.cpp
    changetracker/KoDeletedRowColumnDataStore.cpp
    changetracker/KoDeletedRowData.cpp
    changetracker/KoDeletedColumnData.cpp
    changetracker/KoDeletedCellData.cpp

    commands/ChangeAnchorPropertiesCommand.cpp
    commands/ChangeListCommand.cpp
    commands/ChangeStylesCommand.cpp
    commands/ChangeStylesMacroCommand.cpp
    commands/DeleteAnchorsCommand.cpp
    commands/DeleteAnnotationsCommand.cpp
    commands/DeleteCommand.cpp
    commands/DeleteTableColumnCommand.cpp
    commands/DeleteTableRowCommand.cpp
    commands/InsertNoteCommand.cpp
    commands/InsertTableColumnCommand.cpp
    commands/InsertTableRowCommand.cpp
    commands/ResizeTableCommand.cpp
    commands/InsertInlineObjectCommand.cpp
    commands/ListItemNumberingCommand.cpp
    commands/TextPasteCommand.cpp
    commands/AddTextRangeCommand.cpp
    commands/AddAnnotationCommand.cpp
    commands/ParagraphFormattingCommand.cpp
    commands/RenameSectionCommand.cpp
    commands/NewSectionCommand.cpp
    commands/SplitSectionsCommand.cpp

    KoTextDrag.cpp
    KoTextCommandBase.cpp

    TextDebug.cpp
)

if( SHOULD_BUILD_FEATURE_RDF )
    set(kotext_LIB_SRCS ${kotext_LIB_SRCS}
        KoTextRdfCore.cpp
    )
endif()

add_library(kotext SHARED ${kotext_LIB_SRCS})
generate_export_header(kotext BASE_NAME kotext)

target_link_libraries(kotext
    PUBLIC
        flake
        KF6::TextWidgets
    PRIVATE
        koplugin
        KF6::WindowSystem # KoFind to activate the window with content found
)
if( SHOULD_BUILD_FEATURE_RDF )
    target_link_libraries(kotext PRIVATE ${SOPRANO_LIBRARIES})
endif()

if( FONTCONFIG_FOUND )
    target_link_libraries(kotext PRIVATE Fontconfig::Fontconfig)
endif()

if( FREETYPE_FOUND )
    target_link_libraries(kotext PRIVATE Freetype::Freetype)
endif()


set_target_properties(kotext PROPERTIES
   VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS kotext  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# texteditingplugin.desktop inlinetextobject.desktop

if (SHOULD_BUILD_DEVEL_HEADERS)

install(
    FILES
    KoDocumentRdfBase.h
    KoInlineObject.h
    KoTextEditor.h
    KoTextEditingFactory.h
    KoTextEditingPlugin.h
    KoTextEditingRegistry.h
    KoInlineObjectRegistry.h
    KoInlineObjectFactoryBase.h
    KoBookmark.h
    KoBookmarkManager.h
    KoAnnotationManager.h
    KoInlineTextObjectManager.h
    KoAnchorInlineObject.h
    KoAnchorTextRange.h
    KoTextBlockBorderData.h
    KoTextBlockData.h
    KoTextDocument.h
    KoText.h
    KoTextRange.h
    KoTextRangeManager.h
    KoList.h
    KoTextLocator.h
    KoTextPage.h
    KoTextPaste.h
    KoVariable.h
    KoVariableManager.h
    KoTextRdfCore.h
    KoTextInlineRdf.h
    KoTextMeta.h
    KoTextSoftPageBreak.cpp
    KoSection.h
    KoSectionEnd.h
    KoSectionUtils.h
    KoSectionModel.h
    KoTextCommandBase.h
    KoTextTableTemplate.h

    DESTINATION
    ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel
)

install(
    FILES
    styles/KoCharacterStyle.h
    styles/KoListLevelProperties.h
    styles/KoListStyle.h
    styles/KoParagraphStyle.h
    styles/KoTableColumnStyle.h
    styles/KoTableRowStyle.h
    styles/KoTableCellStyle.h
    styles/KoSectionStyle.h
    styles/KoStyleManager.h
    styles/KoTableStyle.h
    DESTINATION
    ${KDE_INSTALL_INCLUDEDIR}/calligra/styles COMPONENT Devel
)

install(
    FILES
    changetracker/KoChangeTracker.h
    changetracker/KoChangeTrackerElement.h
    changetracker/KoDeletedRowColumnDataStore.h
    changetracker/KoDeletedRowData.cpp
    changetracker/KoDeletedColumnData.cpp
    changetracker/KoDeletedCellData.cpp
    DESTINATION
    ${KDE_INSTALL_INCLUDEDIR}/calligra/changetracker COMPONENT Devel
)

endif()
