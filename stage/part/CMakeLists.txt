project(stagepart)

include_directories( ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${Boost_INCLUDE_DIR})

if (BUILD_TESTING AND NOT WIN32) ## Disable tests in Windows whilst they break builds
    add_subdirectory(tests)
endif()
add_subdirectory(tools)

### calligrastageprivate ###
set( calligrastageprivate_LIB_SRCS
     StageDebug.cpp
     KPrFactory.cpp
     KPrDocument.cpp
     KPrDeclarations.cpp
     KPrPart.cpp
     KPrView.cpp
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
     KPrPresentationToolEventForwarder.cpp
     KPrPresentationDrawStrategy.cpp
     KPrPageSelectStrategyBase.cpp
     KPrPageSelectStrategyFixed.cpp
     KPrPageSelectStrategyActive.cpp
     KPrDurationParser.cpp
     KPrHtmlExport.cpp
     KPrPicturesImport.cpp
     KPrPdfPrintJob.cpp
     KPrSlidesSorterDocumentModel.cpp
     KPrSlidesManagerView.cpp
     KPrCustomSlideShowsModel.cpp
     KPrSlidesSorterItemDelegate.cpp
     KPrPageLayoutWidget.cpp
     KPrPageTransition.cpp

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
     commands/KPrPageTransitionSetCommand.cpp

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

if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT CALLIGRA_FLATPAK)
    set(calligrastageprivate_LIB_SRCS
        ${calligrastageprivate_LIB_SRCS}
        KPrViewAdaptor.cpp
        KPrPresentationToolAdaptor.cpp
    )
endif()

ki18n_wrap_ui(calligrastageprivate_LIB_SRCS
     ui/KPrConfigureSlideShow.ui
     ui/KPrConfigurePresenterView.ui
     ui/KPrPresentationTool.ui
     ui/KPrHtmlExport.ui
   )

add_library(calligrastageprivate SHARED ${calligrastageprivate_LIB_SRCS})
generate_export_header(calligrastageprivate
    BASE_NAME stage
    EXPORT_FILE_NAME stage_generated_export.h
)

target_link_libraries(calligrastageprivate
    PUBLIC
        kopageapp
    PRIVATE
        kowidgets
        kotextlayout
        koplugin
        KF6::Archive
        KF6::IconThemes
        KF6::Completion
        Qt6::Svg
)
if(HAVE_OPENGL)
    target_link_libraries(calligrastageprivate PRIVATE Qt6::OpenGL)
endif()

set_target_properties(calligrastageprivate PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )
install(TARGETS calligrastageprivate  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

### calligrastagepart ###
set(calligrastagepart_PART_SRCS KPrFactoryInit.cpp )

add_library(calligrastagepart MODULE ${calligrastagepart_PART_SRCS})


target_link_libraries(calligrastagepart calligrastageprivate )

install(TARGETS calligrastagepart DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/parts)

### GUI files ###
install( FILES calligrastage.rc calligrastage_readonly.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/calligrastage)

install( FILES calligrastagerc DESTINATION ${KDE_INSTALL_CONFDIR} )

### Predefined Animations ###
install(FILES
        animations/animations.xml
        DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/animations)

### Include files ###
if (SHOULD_BUILD_DEVEL_HEADERS)

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
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligrastage/part
        COMPONENT Devel)

install( FILES 
         animations/KPrAnimationData.h
         animations/KPrAnimationStep.h
         animations/KPrShapeAnimation.h
         DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligrastage/part/animations
         COMPONENT Devel)

install( FILES
         pagelayout/KPrPlaceholders.h
         DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligrastage/part/pagelayout
         COMPONENT Devel)
endif()
