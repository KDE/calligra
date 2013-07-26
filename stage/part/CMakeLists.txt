project(stagepart)

include_directories( ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${Boost_INCLUDE_DIR})

if(KDE4_BUILD_TESTS)
    # only with this definition will the STAGE_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

if (!WIN32) ## Disable tests in Windows whilst they break builds
	add_subdirectory(tests)
endif(!WIN32)
add_subdirectory(tools)

### calligrastageprivate ###
set( calligrastageprivate_LIB_SRCS
     KPrFactory.cpp
     KPrDocument.cpp
     KPrDeclarations.cpp
     KPrPart.cpp
     KPrView.cpp
     KPrViewAdaptor.cpp
     KPrViewModePresentation.cpp
     KPrViewModeNotes.cpp
     KPrViewModeSlidesSorter.cpp
     KPrViewModePreviewPageEffect.cpp
     KPrViewModePreviewShapeAnimations.cpp
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
     KPrPresentationToolAdaptor.cpp
     KPrPresentationToolEventForwarder.cpp
     KPrPresentationDrawStrategy.cpp
     KPrPageSelectStrategyBase.cpp
     KPrPageSelectStrategyFixed.cpp
     KPrPageSelectStrategyActive.cpp
     KPrDurationParser.cpp
     KPrHtmlExport.cpp
     KPrHtmlExportUiDelegate.cpp
     KPrPicturesImport.cpp
     KPrPdfPrintJob.cpp
     KPrSlidesSorterDocumentModel.cpp
     KPrSlidesManagerView.cpp
     KPrCustomSlideShowsModel.cpp
     KPrSlidesSorterItemDelegate.cpp
     KPrPageLayoutWidget.cpp

     commands/KPrAnimationCreateCommand.cpp
     commands/KPrAnimationRemoveCommand.cpp
     commands/KPrPageEffectSetCommand.cpp
     commands/KPrPageLayoutCommand.cpp
     commands/KPrEditCustomSlideShowsCommand.cpp
     commands/KPrAddCustomSlideShowCommand.cpp
     commands/KPrDelCustomSlideShowCommand.cpp
     commands/KPrRenameCustomSlideShowCommand.cpp
     commands/KPrDeleteSlidesCommand.cpp
     commands/KPrEditAnimationTimeLineCommand.cpp
     commands/KPrAnimationEditNodeTypeCommand.cpp
     commands/KPrReorderAnimationCommand.cpp
     commands/KPrReplaceAnimationCommand.cpp

     dockers/KPrPreviewWidget.cpp

     pageeffects/KPrPageEffectRunner.cpp
     pageeffects/KPrPageEffect.cpp
     pageeffects/KPrPageEffectStrategy.cpp
     pageeffects/KPrPageEffectFactory.cpp
     pageeffects/KPrPageEffectRegistry.cpp

     animations/KPrAnimationBase.cpp
     animations/KPrAnimSet.cpp
     animations/KPrAnimate.cpp
     animations/KPrAnimateColor.cpp
     animations/KPrAnimateMotion.cpp
     animations/KPrAnimateTransform.cpp
     animations/KPrAnimTransitionFilter.cpp
     animations/KPrAnimationFactory.cpp
     animations/KPrAnimationCache.cpp
     animations/KPrTextBlockPaintStrategy.cpp
     animations/KPrShapeAnimation.cpp
     animations/KPrAnimationStep.cpp
     animations/KPrAnimationSubStep.cpp
     animations/KPrAnimationLoader.cpp
     animations/KPrAnimationData.cpp

     animations/strategy/KPrAnimationValue.cpp
     animations/strategy/KPrFormulaParser.cpp
     animations/strategy/KPrAnimationAttribute.cpp
     animations/strategy/KPrSmilValues.cpp
     animations/strategy/KPrAttributeX.cpp
     animations/strategy/KPrAttributeY.cpp
     animations/strategy/KPrAttributeWidth.cpp
     animations/strategy/KPrAttributeHeight.cpp
     animations/strategy/KPrAttributeRotate.cpp

     pagelayout/KPrPlaceholder.cpp
     pagelayout/KPrPageLayout.cpp
     pagelayout/KPrPageLayouts.cpp
     pagelayout/KPrPageLayoutSharedSavingData.cpp
     pagelayout/KPrPlaceholders.cpp

     ui/KPrConfigureSlideShowDialog.cpp
     ui/KPrConfigurePresenterViewDialog.cpp
     ui/KPrPresentationToolWidget.cpp
     ui/KPrHtmlExportDialog.cpp

     tools/KPrPlaceholderTool.cpp
     tools/KPrPlaceholderToolFactory.cpp
   )

kde4_add_ui_files(calligrastageprivate_LIB_SRCS
     ui/KPrConfigureSlideShow.ui
     ui/KPrConfigurePresenterView.ui
     ui/KPrPresentationTool.ui
     ui/KPrHtmlExport.ui
   )

kde4_add_library(calligrastageprivate SHARED ${calligrastageprivate_LIB_SRCS})

target_link_libraries(calligrastageprivate kopageapp kowidgets kotextlayout ${KDE4_PHONON_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTWEBKIT_LIBRARY})
target_link_libraries(calligrastageprivate LINK_INTERFACE_LIBRARIES kopageapp)

set_target_properties(calligrastageprivate PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS calligrastageprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

### calligrastagepart ###
set(calligrastagepart_PART_SRCS KPrFactoryInit.cpp )

kde4_add_plugin(calligrastagepart ${calligrastagepart_PART_SRCS})



target_link_libraries(calligrastagepart calligrastageprivate )

install(TARGETS calligrastagepart DESTINATION ${PLUGIN_INSTALL_DIR})

### desktop files ###
install( FILES stagepart.desktop DESTINATION ${SERVICES_INSTALL_DIR})

### GUI files ###
install( FILES stage.rc stage_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/stage)

install( FILES stagerc DESTINATION ${CONFIG_INSTALL_DIR} )

### Predefined Animations ###
install(FILES
        animations/animations.xml
        DESTINATION ${DATA_INSTALL_DIR}/stage/animations)

### Include files ###
install( FILES
        stage_export.h
        KPrAnimationDirector.h
        KPrCustomSlideShows.h
        KPrDocument.h  
        KPrPage.h
        KPrPageData.h
        KPrDeclarations.h
        KPrPresentationTool.h
        KPrNotes.h
        KPrShapeAnimations.h
        KPrView.h
        KPrViewModePresentation.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/stage/part)

install( FILES 
         animations/KPrAnimationData.h
         animations/KPrAnimationStep.h
         animations/KPrShapeAnimation.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/stage/part/animations)

install( FILES
         pagelayout/KPrPlaceholders.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/stage/part/pagelayout)
