project(wordspart)

#TODO port to textlayout-rework
add_subdirectory( tests )

include_directories( ${KOTEXT_INCLUDES} ${TEXTLAYOUT_INCLUDES} ${KDE4_INCLUDE_DIR}/threadweaver/)

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
    frames/KWOutlineShape.cpp

    dialogs/KWFrameConnectSelector.cpp
    dialogs/KWRunAroundProperties.cpp
    dialogs/KWGeneralFrameProperties.cpp
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
    dockers/KWStatistics.cpp
    dockers/StatisticsPreferencesPopup.cpp

    commands/KWPageStylePropertiesCommand.cpp
    commands/KWPageInsertCommand.cpp
    commands/KWPageRemoveCommand.cpp
    commands/KWNewPageStyleCommand.cpp
    commands/KWFrameDeleteCommand.cpp
    commands/KWFrameCreateCommand.cpp
    commands/KWChangePageStyleCommand.cpp
    commands/KWShapeCreateCommand.cpp
)
if( Soprano_FOUND )
    set(wordsprivate_LIB_SRCS ${wordsprivate_LIB_SRCS}
        dockers/KWRdfDocker.cpp
        dockers/KWRdfDockerFactory.cpp
        dockers/KWRdfDockerTree.cpp
    )
endif( Soprano_FOUND )

kde4_add_ui_files(wordsprivate_LIB_SRCS
    dialogs/KWFrameConnectSelector.ui
    dialogs/KWRunAroundProperties.ui
    dialogs/KWGeneralFrameProperties.ui
    dialogs/KWAnchoringProperties.ui
    dialogs/KWDocumentColumns.ui
    dialogs/KWStartupWidget.ui
    dialogs/KWCreateBookmark.ui
    dialogs/KWSelectBookmark.ui
    dialogs/KWInsertImage.ui
    dockers/KWStatisticsDocker.ui
    dockers/StatisticsPreferencesPopup.ui
    pagetool/SimpleSetupWidget.ui
    pagetool/SimpleHeaderFooterWidget.ui
)
if( Soprano_FOUND )
    kde4_add_ui_files(wordsprivate_LIB_SRCS
        dockers/KWRdfDocker.ui
    )
endif( Soprano_FOUND )


kde4_add_library(wordsprivate SHARED ${wordsprivate_LIB_SRCS})

target_link_libraries(wordsprivate komain textlayout)
if( Soprano_FOUND )
    target_link_libraries(wordsprivate ${SOPRANO_LIBRARIES})
endif( Soprano_FOUND )
target_link_libraries(wordsprivate LINK_INTERFACE_LIBRARIES komain)

set_target_properties(wordsprivate
    PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS wordsprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### WordsPart Library ###############

set(wordspart_PART_SRCS KWFactoryInit.cpp )


kde4_add_plugin(wordspart ${wordspart_PART_SRCS})



target_link_libraries(wordspart ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} wordsprivate )

install(TARGETS wordspart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### Words exe ###############

set(words_KDEINIT_SRCS main.cpp )

kde4_add_app_icon(words_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/hi*-app-words.png")

kde4_add_kdeinit_executable( calligrawords ${words_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.words")
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Words")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_calligrawords  ${KDE4_KDEUI_LIBS} komain )

install(TARGETS kdeinit_calligrawords  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligrawords kdeinit_calligrawords )
install(TARGETS calligrawords  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES words.rc words_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/words)
install( FILES wordspart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS words.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
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

