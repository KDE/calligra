project(wordspart)

#TODO port to textlayout-rework
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()

include_directories( ${KORDF_INCLUDES} ${KOTEXT_INCLUDES} ${TEXTLAYOUT_INCLUDES})

########### WordsPrivate library ###############

set(wordsprivate_LIB_SRCS
    KWFactory.cpp
    WordsDebug.cpp
    Words.cpp
    KWApplicationConfig.cpp
    KWCanvasBase.cpp
    KWCanvas.cpp
    KWCanvasItem.cpp
    KWDocument.cpp
    KWDocumentStatistics.cpp
    KWGui.cpp
    KWView.cpp
    KWPart.cpp
    KWPage.cpp
    KWPageCacheManager.cpp
    KWPageManager.cpp
    KWPageStyle.cpp
    KWViewMode.cpp
    KWViewModeNormal.cpp
    KWViewModePreview.cpp
    KWStatusBar.cpp
    KWOdfLoader.cpp
    KWOdfWriter.cpp
    KWOdfSharedLoadingData.cpp
    KWRootAreaProviderBase.cpp
    KWRootAreaProviderTextBox.cpp
    KWRootAreaProvider.cpp

    pagetool/KWPageTool.cpp
    pagetool/KWPageToolFactory.cpp
    pagetool/SimpleSetupWidget.cpp
    pagetool/SimpleHeaderFooterWidget.cpp

    frames/KWFrameLayout.cpp
    frames/KWFrame.cpp
    frames/KWFrameSet.cpp
    frames/KWTextFrameSet.cpp
    frames/KWCopyShape.cpp

    widgets/KoFindToolbar.cpp

    dialogs/KWFrameConnectSelector.cpp
    dialogs/KWRunAroundProperties.cpp
    dialogs/KWAnchoringProperties.cpp
    dialogs/KWFrameDialog.cpp
    dialogs/KWShapeConfigFactory.cpp
    dialogs/KWDocumentColumns.cpp
    dialogs/KWStartupWidget.cpp
    dialogs/KWPageSettingsDialog.cpp
    dialogs/KWPrintingDialog.cpp
    dialogs/KWCreateBookmarkDialog.cpp
    dialogs/KWSelectBookmarkDialog.cpp
    dialogs/KWConfigureDialog.cpp

    dockers/KWStatisticsDocker.cpp
    dockers/KWStatisticsWidget.cpp
    dockers/StatisticsPreferencesPopup.cpp

    dockers/KWNavigationDocker.cpp
    dockers/KWNavigationDockerFactory.cpp
    dockers/KWNavigationWidget.cpp

    commands/KWPageStylePropertiesCommand.cpp
    commands/KWNewPageStyleCommand.cpp
    commands/KWChangePageStyleCommand.cpp
    commands/KWShapeCreateCommand.cpp
)

set(wordsprivate_LIB_SRCS ${wordsprivate_LIB_SRCS}
    dockers/KWDebugDocker.cpp
    dockers/KWDebugDockerFactory.cpp
    dockers/KWDebugWidget.cpp
)

if( SHOULD_BUILD_FEATURE_RDF )
    set(wordsprivate_LIB_SRCS ${wordsprivate_LIB_SRCS}
        dockers/KWRdfDocker.cpp
        dockers/KWRdfDockerFactory.cpp
        dockers/KWRdfDockerTree.cpp
    )

endif()

ki18n_wrap_ui(wordsprivate_LIB_SRCS
    dialogs/KWFrameConnectSelector.ui
    dialogs/KWRunAroundProperties.ui
    dialogs/KWAnchoringProperties.ui
    dialogs/KWDocumentColumns.ui
    dialogs/KWStartupWidget.ui
    dialogs/KWCreateBookmark.ui
    dialogs/KWSelectBookmark.ui
    dialogs/KWInsertImage.ui
    dockers/StatisticsPreferencesPopup.ui
    pagetool/SimpleSetupWidget.ui
    pagetool/SimpleHeaderFooterWidget.ui
)
if( SHOULD_BUILD_FEATURE_RDF )
    ki18n_wrap_ui(wordsprivate_LIB_SRCS
        dockers/KWRdfDocker.ui
    )
endif()

add_library(wordsprivate SHARED ${wordsprivate_LIB_SRCS})
generate_export_header(wordsprivate
    BASE_NAME words
    EXPORT_FILE_NAME words_generated_export.h
    )


target_link_libraries(wordsprivate
    PUBLIC
        komain
    PRIVATE
        kotextlayout
        KF6::IconThemes
        KF6::Completion
)
if( SHOULD_BUILD_FEATURE_RDF )
    target_link_libraries(wordsprivate
        PRIVATE
            kordf
    )
endif()

set_target_properties(wordsprivate
    PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS wordsprivate  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### WordsPart Library ###############

set(wordspart_PART_SRCS KWFactoryInit.cpp )


add_library(calligrawordspart MODULE ${wordspart_PART_SRCS})


target_link_libraries(calligrawordspart   wordsprivate KF6::CoreAddons)

install(TARGETS calligrawordspart  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/parts)


########### install files ###############

install( FILES calligrawords.rc calligrawords_readonly.rc  DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/calligrawords)
install( FILES calligrawordsrc DESTINATION ${KDE_INSTALL_CONFDIR} )

if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
words_export.h
KWPage.h
KWPageManager.h
KWPageStyle.h
Words.h
KWCanvasBase.h
KWCanvas.h
KWCanvasItem.h
KWDocument.h
KWDocumentStatistics.h
KWApplicationConfig.h
KWViewMode.h
KWView.h
KWViewModeNormal.h
DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligrawords/part COMPONENT Devel)

install( FILES
frames/KWFrame.h
frames/KWFrameLayout.h
frames/KWFrameSet.h
frames/KWTextFrameSet.h
DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligrawords/part/frames COMPONENT Devel)

endif()

