project(kpresenter)

include_directories( ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${Boost_INCLUDE_DIR})

if(KDE4_BUILD_TESTS)
    # only with this definition will the KPRESENTER_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)


add_subdirectory(tests)
add_subdirectory(tools)

### kpresenterprivate ###
set( kpresenterprivate_LIB_SRCS
     KPrFactory.cpp
     KPrDocument.cpp
     KPrDeclarations.cpp
     KPrView.cpp
     KPrViewAdaptor.cpp
     KPrViewModePresentation.cpp
     KPrViewModeNotes.cpp
     KPrViewModePreviewPageEffect.cpp
     KPrPresentationTool.cpp
     KPrAnimationDirector.cpp
     KPrShapeAnimations.cpp
     KPrShapeManagerAnimationStrategy.cpp
     KPrShapeManagerDisplayMasterStrategy.cpp
     KPrPageData.cpp
     KPrPage.cpp
     KPrMasterPage.cpp
     KPrNotes.cpp
     KPrSoundData.cpp
     KPrSoundCollection.cpp
     KPrEventActionData.cpp
     KPrEventActionWidget.cpp
     KPrPageApplicationData.cpp
     KPrShapeApplicationData.cpp
     KPrCustomSlideShows.cpp
     KPrPresenterViewBaseInterface.cpp
     KPrPresenterViewInterface.cpp
     KPrPresenterViewSlidesInterface.cpp
     KPrPresenterViewToolWidget.cpp
     KPrPresenterViewWidget.cpp
     KPrEndOfSlideShowPage.cpp
     KPrPlaceholderShape.cpp
     KPrPlaceholderShapeFactory.cpp
     KPrPlaceholderStrategy.cpp
     KPrPlaceholderPictureStrategy.cpp
     KPrPlaceholderTextStrategy.cpp
     KPrPresentationHighlightWidget.cpp
     KPrPresentationDrawWidget.cpp
     KPrPresentationBlackWidget.cpp
     KPrPresentationStrategy.cpp
     KPrPresentationHighlightStrategy.cpp
     KPrPresentationBlackStrategy.cpp
     KPrPresentationStrategyBase.cpp
     KPrPresentationDrawStrategy.cpp
     KPrPageSelectStrategyBase.cpp
     KPrPageSelectStrategyFixed.cpp
     KPrPageSelectStrategyActive.cpp

     commands/KPrAnimationCreateCommand.cpp
     commands/KPrPageEffectSetCommand.cpp
     commands/KPrPageLayoutCommand.cpp
     commands/KPrSetCustomSlideShowsCommand.cpp

     dockers/KPrPreviewWidget.cpp
     dockers/KPrPageLayoutDockerFactory.cpp
     dockers/KPrPageLayoutDocker.cpp

     pageeffects/KPrPageEffectRunner.cpp
     pageeffects/KPrPageEffect.cpp
     pageeffects/KPrPageEffectStrategy.cpp
     pageeffects/KPrPageEffectFactory.cpp
     pageeffects/KPrPageEffectRegistry.cpp

     shapeanimations/KPrShapeAnimationOld.cpp
     shapeanimations/KPrTranslateAnimation.cpp
     shapeanimations/KPrAnimationMoveAppear.cpp
     shapeanimations/KPrAnimationDisappear.cpp
     shapeanimations/KPrShapeAnimationFactory.cpp
     shapeanimations/KPrShapeAnimationRegistry.cpp

     animations/KPrAnimationBase.cpp
     animations/KPrAnimSet.cpp
     animations/KPrAnimate.cpp
     animations/KPrAnimateColor.cpp
     animations/KPrAnimateMotion.cpp
     animations/KPrAnimateTransform.cpp
     animations/KPrAnimationFactory.cpp
     animations/KPrAnimationCache.cpp
     animations/KPrTextBlockPaintStrategy.cpp
     animations/KPrShapeAnimation.cpp
     animations/KPrAnimationStep.cpp
     animations/KPrAnimationSubStep.cpp
     animations/KPrAnimationLoader.cpp

     pagelayout/KPrPlaceholder.cpp
     pagelayout/KPrPageLayout.cpp
     pagelayout/KPrPageLayouts.cpp
     pagelayout/KPrPageLayoutSharedSavingData.cpp
     pagelayout/KPrPlaceholders.cpp

     ui/KPrCustomSlideShowsDialog.cpp
     ui/KPrConfigureSlideShowDialog.cpp
     ui/KPrConfigurePresenterViewDialog.cpp
     ui/KPrPresentationToolWidget.cpp

     tools/KPrPlaceholderTool.cpp
     tools/KPrPlaceholderToolFactory.cpp
   )

kde4_add_ui_files(kpresenterprivate_LIB_SRCS
     ui/KPrCustomSlideShowsDialog.ui
     ui/KPrConfigureSlideShow.ui
     ui/KPrConfigurePresenterView.ui
     ui/KPrPresentationTool.ui
   )

kde4_add_library(kpresenterprivate SHARED ${kpresenterprivate_LIB_SRCS})

target_link_libraries(kpresenterprivate kopageapp kowidgets ${KDE4_PHONON_LIBRARY} ${QT_QTOPENGL_LIBRARY})
target_link_libraries(kpresenterprivate LINK_INTERFACE_LIBRARIES kopageapp)

set_target_properties(kpresenterprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kpresenterprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

### kpresenterpart ###
set(kpresenterpart_PART_SRCS KPrFactoryInit.cpp )

kde4_add_plugin(kpresenterpart WITH_PREFIX ${kpresenterpart_PART_SRCS})



target_link_libraries(kpresenterpart kpresenterprivate )

install(TARGETS kpresenterpart DESTINATION ${PLUGIN_INSTALL_DIR})

### kdeinit kpresenter ###
set(kpresenter_KDEINIT_SRCS main.cpp )

kde4_add_app_icon(kpresenter_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/hi*-app-kpresenter.png")

kde4_add_kdeinit_executable( kpresenter ${kpresenter_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(kpresenter PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(kpresenter PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.koffice.kpresenter")
   set_target_properties(kpresenter PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KPresenter 2")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_kpresenter komain )

install(TARGETS kdeinit_kpresenter ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(kpresenter kdeinit_kpresenter)
install(TARGETS kpresenter ${INSTALL_TARGETS_DEFAULT_ARGS})

### desktop files ###
install( FILES kpresenterpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kpresenter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

### GUI files ###
install( FILES kpresenter.rc kpresenter_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/kpresenter)

install( FILES kpresenterrc DESTINATION ${CONFIG_INSTALL_DIR} )
