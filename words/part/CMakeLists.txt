project(wordspart)

#TODO port to textlayout-rework
add_subdirectory( tests )

include_directories( ${KORDF_INCLUDES} ${KOTEXT_INCLUDES} ${TEXTLAYOUT_INCLUDES})

########### WordsPrivate library ###############

set(wordsprivate_LIB_SRCS
    KWFactory.cpp
    Words.cpp
    KWApplicationConfig.cpp
    KWCanvasBase.cpp
    KWCanvas.cpp
    KWCanvasItem.cpp
    KWDocument.cpp
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

if (SHOULD_BUILD_APP_AUTHOR)
set(wordsprivate_LIB_SRCS ${wordsprivate_LIB_SRCS}
##calligra author specific additions
    author/CAuFactory.cpp
    author/CAuView.cpp
    author/CAuPart.cpp
    author/CoverImage.cpp
    author/CoverSelectionDialog.cpp
    author/CAuDocument.cpp
)
endif()

if( SHOULD_BUILD_FEATURE_RDF )
# TODO: depends hard on Soprano
set(wordsprivate_LIB_SRCS ${wordsprivate_LIB_SRCS}
    dockers/KWDebugDocker.cpp
    dockers/KWDebugDockerFactory.cpp
    dockers/KWDebugWidget.cpp
)
endif()

if( SHOULD_BUILD_FEATURE_RDF )
    set(wordsprivate_LIB_SRCS ${wordsprivate_LIB_SRCS}
        dockers/KWRdfDocker.cpp
        dockers/KWRdfDockerFactory.cpp
        dockers/KWRdfDockerTree.cpp
    )

    if (SHOULD_BUILD_APP_AUTHOR)
    set(wordsprivate_LIB_SRCS ${wordsprivate_LIB_SRCS}
        author/dockers/CAuOutlinerDocker.cpp
        author/dockers/CAuOutlinerDockerFactory.cpp
        author/dockers/CAuOutlinerWidget.cpp

        author/metadata/CAuMetaDataManager.cpp

        author/metadata/CAuSectionRdf.cpp
        author/metadata/CAuSectionSemanticItemFactory.cpp

        author/metadata/CAuActorRdf.cpp
        author/metadata/CAuActorSemanticItemFactory.cpp

        author/metadata/CAuSemanticItemBase.cpp
    )
    endif()
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
    author/CoverSelectionDialog.ui
)
if( SHOULD_BUILD_FEATURE_RDF )
    ki18n_wrap_ui(wordsprivate_LIB_SRCS
        dockers/KWRdfDocker.ui
        author/metadata/CAuSectionRdfEditWidget.ui
        author/metadata/CAuActorRdfEditWidget.ui
    )
endif()


add_library(wordsprivate SHARED ${wordsprivate_LIB_SRCS})

target_link_libraries(wordsprivate komain kotextlayout)
if( SHOULD_BUILD_FEATURE_RDF )
    target_link_libraries(wordsprivate kordf)
endif()
target_link_libraries(wordsprivate LINK_INTERFACE_LIBRARIES komain)

set_target_properties(wordsprivate
    PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS wordsprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### WordsPart Library ###############

set(wordspart_PART_SRCS KWFactoryInit.cpp )


add_library(wordspart MODULE ${wordspart_PART_SRCS})

kcoreaddons_desktop_to_json(wordspart wordspart.desktop)

target_link_libraries(wordspart   wordsprivate )

install(TARGETS wordspart  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES words.rc words_readonly.rc  DESTINATION ${KXMLGUI_INSTALL_DIR}/words)
install( FILES wordsrc DESTINATION ${CONFIG_INSTALL_DIR} )

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
KWApplicationConfig.h
KWViewMode.h
KWView.h
KWViewModeNormal.h
DESTINATION ${INCLUDE_INSTALL_DIR}/words/part COMPONENT Devel)

install( FILES
frames/KWFrame.h
frames/KWFrameLayout.h
frames/KWFrameSet.h
frames/KWTextFrameSet.h
DESTINATION ${INCLUDE_INSTALL_DIR}/words/part/frames COMPONENT Devel)


###What follows is calligraauthor section###

if (SHOULD_BUILD_APP_AUTHOR)

########### AuthorPart Library ###############

set(authorpart_PART_SRCS author/CAuFactoryInit.cpp )

add_library(authorpart MODULE ${authorpart_PART_SRCS})

kcoreaddons_desktop_to_json(authorpart author/authorpart.desktop)

target_link_libraries(authorpart   wordsprivate )

install(TARGETS authorpart DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### Author exe ###############

set(author_KDEINIT_SRCS author/main.cpp )

#kde4_add_app_icon(author_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/author/hi*-app-calligraauthor.png")

kf5_add_kdeinit_executable( calligraauthor ${author_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(calligraauthor PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../app/Info.plist.template)
   set_target_properties(calligraauthor PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.author")
   set_target_properties(calligraauthor PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Author")
endif ()

target_link_libraries(kdeinit_calligraauthor   komain )

install(TARGETS kdeinit_calligraauthor  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligraauthor kdeinit_calligraauthor )
install(TARGETS calligraauthor  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES author/author.rc author/author_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/author)
install( FILES author/authorpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install( PROGRAMS author/author.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES author/authorrc DESTINATION ${CONFIG_INSTALL_DIR} )

endif ()
