project(kwordpart)

add_subdirectory( frames/tests )
add_subdirectory( tests )

include_directories( ${KOTEXT_INCLUDES} ${KOGUIUTILS_INCLUDES} ${KDE4_INCLUDE_DIR}/threadweaver/ )

########### KWordPrivate library ###############

set(kwordprivate_LIB_SRCS
    KWFactory.cpp
    KWApplicationConfig.cpp
    KWCanvas.cpp
    KWDocument.cpp
    KWGui.cpp
    KWView.cpp
    KWPage.cpp
    KWPageManager.cpp
    KWPageSettings.cpp
    KWViewMode.cpp
    KWViewModeNormal.cpp
    KWViewModePreview.cpp
    KWStatusBar.cpp
    KWOpenDocumentLoader.cpp
    KWDLoader.cpp

    frames/KWTextDocumentLayout.cpp
    frames/KWFrameLayout.cpp
    frames/KWFrame.cpp
    frames/KWFrameSet.cpp
    frames/KWTextFrame.cpp
    frames/KWTableFrame.cpp
    frames/KWTextFrameSet.cpp
    frames/KWCopyShape.cpp
    frames/KWAnchorStrategy.cpp
    frames/KWImageFrame.cpp
    frames/KWOutlineShape.cpp

    dialogs/KWFrameConnectSelector.cpp
    dialogs/KWFrameGeometry.cpp
    dialogs/KWFrameRunaroundProperties.cpp
    dialogs/KWGeneralFrameProperties.cpp
    dialogs/KWFrameDialog.cpp
    dialogs/KWShapeConfigFactory.cpp
    dialogs/KWDocumentColumns.cpp
    dialogs/KWPagePreview.cpp
    dialogs/KWPageLayout.cpp
    dialogs/KWStartupWidget.cpp
    dialogs/KWPageSettingsDialog.cpp
    dialogs/KWStatistics.cpp
    dialogs/KWStatisticsDialog.cpp
    dialogs/KWPrintingDialog.cpp

    dockers/KWStatisticsDocker.cpp
#    dockers/KWPagesDocker.cpp

    commands/KWPagePropertiesCommand.cpp
    commands/KWPageInsertCommand.cpp
    commands/KWPageRemoveCommand.cpp
    commands/KWFrameDeleteCommand.cpp
    commands/KWFrameCreateCommand.cpp
    commands/KWCreateOutlineCommand.cpp
)

kde4_add_ui_files(kwordprivate_LIB_SRCS
    dialogs/KWFrameConnectSelector.ui
    dialogs/KWFrameGeometry.ui
    dialogs/KWFrameRunaroundProperties.ui
    dialogs/KWGeneralFrameProperties.ui
    dialogs/KWDocumentColumns.ui
    dialogs/KWPageLayout.ui
    dialogs/KWStartupWidget.ui
    dialogs/KWStatistics.ui
    dockers/KWStatisticsDocker.ui
)

kde4_add_library(kwordprivate SHARED ${kwordprivate_LIB_SRCS})

target_link_libraries(kwordprivate  ${KDE4_KDECORE_LIBS} flake koguiutils kotext)

if(KDE4_BUILD_TESTS)
    # only with this definition will the KWORD_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

set_target_properties(kwordprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kwordprivate  DESTINATION ${LIB_INSTALL_DIR})

########### KWordPart Library ###############

set(kwordpart_PART_SRCS KWFactoryInit.cpp )


kde4_add_plugin(kwordpart WITH_PREFIX ${kwordpart_PART_SRCS})



target_link_libraries(kwordpart ${KDE4_KDECORE_LIBS} kwordprivate )

install(TARGETS kwordpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### KWord exe ###############

set(kword_KDEINIT_SRCS main.cpp )

kde4_add_kdeinit_executable( kword ${kword_KDEINIT_SRCS})

target_link_libraries(kdeinit_kword  ${KDE4_KDECORE_LIBS} komain )

install(TARGETS kdeinit_kword  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kword kdeinit_kword )
install(TARGETS kword  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kword.rc kword_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kword)
install( FILES  kwordpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kword.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
