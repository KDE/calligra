project(flake)

include_directories(${FLAKE_INCLUDES} )

add_subdirectory(styles)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
add_subdirectory(pics)

set(flake_SRCS
    KoGradientHelper.cpp
    KoFlake.cpp
    KoCanvasBase.cpp
    KoResourceManager_p.cpp
    KoCanvasResourceManager.cpp
    KoDocumentResourceManager.cpp
    KoCanvasObserverBase.cpp
    KoCanvasSupervisor.cpp
    KoCutController.cpp
    KoCopyController.cpp
    KoDockFactoryBase.cpp
    KoDockRegistry.cpp
    KoDataCenterBase.cpp
    KoInsets.cpp
    KoPasteController.cpp
    KoGuidesData.cpp
    KoGridData.cpp
    KoPathShape.cpp
    KoPathPoint.cpp
    KoPathSegment.cpp
    KoSelection.cpp
    KoShape.cpp
    KoShapeAnchor.cpp
    KoShapeBasedDocumentBase.cpp
    KoShapeApplicationData.cpp
    KoShapeContainer.cpp
    KoShapeContainerModel.cpp
    KoShapeContainerDefaultModel.cpp
    KoShapeGroup.cpp
    KoShapeManagerPaintingStrategy.cpp
    KoShapeManager.cpp
    KoShapePaintingContext.cpp
    KoShapePainter.cpp
    KoFrameShape.cpp
    KoUnavailShape.cpp
    KoMarkerData.cpp
    KoMarker.cpp
    KoMarkerCollection.cpp
    KoMarkerSharedLoadingData.cpp
    #KoUnavailData.cpp
    #KoUnavailCollection.cpp
    KoToolBase.cpp
    KoCanvasController.cpp
    KoCanvasControllerWidget.cpp
    KoCanvasControllerWidgetViewport_p.cpp
    KoShapeRegistry.cpp
    KoDeferredShapeFactoryBase.cpp
    KoToolFactoryBase.cpp
    KoPathShapeFactory.cpp
    KoShapeFactoryBase.cpp
    KoShapeUserData.cpp
    KoParameterShape.cpp
    KoPointerEvent.cpp
    KoShapeController.cpp
    KoToolSelection.cpp
    KoShapeLayer.cpp
    KoPostscriptPaintDevice.cpp
    KoInputDevice.cpp
    KoToolManager_p.cpp
    KoToolManager.cpp
    KoToolRegistry.cpp
    KoToolProxy.cpp
    KoShapeSavingContext.cpp
    KoShapeLoadingContext.cpp
    KoLoadingShapeUpdater.cpp
    KoPathShapeLoader.cpp
    KoShapeStrokeModel.cpp
    KoShapeStroke.cpp
    KoShapeBackground.cpp
    KoColorBackground.cpp
    KoGradientBackground.cpp
    KoOdfGradientBackground.cpp
    KoHatchBackground.cpp
    KoPatternBackground.cpp
    KoShapeConfigWidgetBase.cpp
    KoShapeConfigFactoryBase.cpp
    KoDrag.cpp
    KoDragOdfSaveHelper.cpp
    KoShapeOdfSaveHelper.cpp
    KoShapePaste.cpp
    KoConnectionPoint.cpp
    KoConnectionShape.cpp
    KoConnectionShapeLoadingUpdater.cpp
    KoConnectionShapeFactory.cpp
    KoConnectionShapeConfigWidget.cpp
    KoSnapGuide.cpp
    KoSnapProxy.cpp
    KoSnapStrategy.cpp
    KoSnapData.cpp
    SnapGuideConfigWidget.cpp
    KoShapeShadow.cpp
    KoSharedLoadingData.cpp
    KoSharedSavingData.cpp
    KoViewConverter.cpp
    KoInputDeviceHandler.cpp
    KoInputDeviceHandlerEvent.cpp
    KoInputDeviceHandlerRegistry.cpp
    KoEventAction.cpp
    KoEventActionFactoryBase.cpp
    KoEventActionRegistry.cpp
    KoImageData.cpp
    KoImageData_p.cpp
    KoImageCollection.cpp
    KoOdfWorkaround.cpp
    KoFilterEffect.cpp
    KoFilterEffectStack.cpp
    KoFilterEffectFactoryBase.cpp
    KoFilterEffectRegistry.cpp
    KoFilterEffectConfigWidgetBase.cpp
    KoFilterEffectRenderContext.cpp
    KoFilterEffectLoadingContext.cpp
    KoTextShapeDataBase.cpp
    KoTosContainer.cpp
    KoTosContainerModel.cpp
    KoClipPath.cpp
    KoCurveFit.cpp
    KoAnnotationLayoutManager.cpp
    commands/KoShapeGroupCommand.cpp
    commands/KoShapeAlignCommand.cpp
    commands/KoShapeBackgroundCommand.cpp
    commands/KoShapeCreateCommand.cpp
    commands/KoShapeDeleteCommand.cpp
    commands/KoShapeDistributeCommand.cpp
    commands/KoShapeLockCommand.cpp
    commands/KoShapeMoveCommand.cpp
    commands/KoShapeShearCommand.cpp
    commands/KoShapeSizeCommand.cpp
    commands/KoShapeStrokeCommand.cpp
    commands/KoShapeUngroupCommand.cpp
    commands/KoShapeReorderCommand.cpp
    commands/KoShapeKeepAspectRatioCommand.cpp
    commands/KoPathBaseCommand.cpp
    commands/KoPathPointMoveCommand.cpp
    commands/KoPathControlPointMoveCommand.cpp
    commands/KoPathPointTypeCommand.cpp
    commands/KoPathPointRemoveCommand.cpp
    commands/KoPathPointInsertCommand.cpp
    commands/KoPathSegmentBreakCommand.cpp
    commands/KoPathBreakAtPointCommand.cpp
    commands/KoPathSegmentTypeCommand.cpp
    commands/KoPathCombineCommand.cpp
    commands/KoSubpathRemoveCommand.cpp
    commands/KoSubpathJoinCommand.cpp
    commands/KoParameterHandleMoveCommand.cpp
    commands/KoParameterToPathCommand.cpp
    commands/KoShapeTransformCommand.cpp
    commands/KoPathFillRuleCommand.cpp
    commands/KoConnectionShapeTypeCommand.cpp
    commands/KoShapeShadowCommand.cpp
    commands/KoPathReverseCommand.cpp
    commands/KoEventActionAddCommand.cpp
    commands/KoEventActionRemoveCommand.cpp
    commands/KoShapeRenameCommand.cpp
    commands/KoShapeRunAroundCommand.cpp
    commands/KoPathPointMergeCommand.cpp
    commands/KoShapeTransparencyCommand.cpp
    commands/KoShapeClipCommand.cpp
    commands/KoShapeUnclipCommand.cpp
    commands/KoPathShapeMarkerCommand.cpp
    commands/KoShapeConnectionChangeCommand.cpp
    tools/KoCreateShapeStrategy.cpp
    tools/KoPathToolFactory.cpp
    tools/KoPathTool.cpp
    tools/KoPathToolSelection.cpp
    tools/KoPathToolHandle.cpp
    tools/PathToolOptionWidget.cpp
    tools/KoPathPointRubberSelectStrategy.cpp
    tools/KoPathPointMoveStrategy.cpp
    tools/KoPathConnectionPointStrategy.cpp
    tools/KoPathControlPointMoveStrategy.cpp
    tools/KoParameterChangeStrategy.cpp
    tools/KoZoomTool.cpp
    tools/KoZoomToolFactory.cpp
    tools/KoZoomToolWidget.cpp
    tools/KoZoomStrategy.cpp
    tools/KoPanTool.cpp
    tools/KoPanToolFactory.cpp
    tools/KoInteractionTool.cpp
    tools/KoInteractionStrategy.cpp
    tools/KoCreateShapesTool.cpp
    tools/KoCreateShapesToolFactory.cpp
    tools/KoShapeRubberSelectStrategy.cpp
    tools/KoPathSegmentChangeStrategy.cpp
    svg/SvgUtil.cpp
    svg/SvgGraphicContext.cpp
    svg/SvgSavingContext.cpp
    svg/SvgWriter.cpp
    svg/SvgStyleWriter.cpp
    svg/SvgShape.cpp
    svg/SvgParser.cpp
    svg/SvgStyleParser.cpp
    svg/SvgGradientHelper.cpp
    svg/SvgPatternHelper.cpp
    svg/SvgFilterHelper.cpp
    svg/SvgCssHelper.cpp
    svg/SvgClipPathHelper.cpp
    svg/SvgLoadingContext.cpp
    svg/SvgShapeFactory.cpp

    FlakeDebug.cpp
)

ki18n_wrap_ui(flake_SRCS
    tools/PathToolOptionWidgetBase.ui
    KoConnectionShapeConfigWidget.ui
    SnapGuideConfigWidget.ui
    tools/KoZoomToolWidget.ui
)

add_library(flake SHARED ${flake_SRCS})
generate_export_header(flake BASE_NAME flake)

target_link_libraries(flake
    PUBLIC
        pigmentcms
        kowidgetutils
        koodf
        kundo2
    PRIVATE
        koplugin
        KF6::IconThemes
        KF6::ConfigCore
        KF6::WidgetsAddons
        KF6::CoreAddons
        Qt6::Svg
)

set_target_properties(flake PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)

########### install files ###############

install(TARGETS flake  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
#     flake.desktop
#     flaketool.desktop
#     flakeshape.desktop
#     flakedevice.desktop
#     presentationeventaction.desktop
#     scripteventaction.desktop
#     filtereffect.desktop

if (SHOULD_BUILD_DEVEL_HEADERS)

install(
    FILES
    KoCanvasBase.h
    KoCanvasController.h
    KoCanvasControllerWidget.h
    KoCanvasObserverBase.h
    KoCanvasSupervisor.h
    KoCanvasResourceManager.h
    KoDocumentResourceManager.h
    KoConnectionPoint.h
    KoCopyController.h
    KoCutController.h
    KoDataCenterBase.h
    KoDockFactoryBase.h
    KoDockRegistry.h
    KoDeferredShapeFactoryBase.h
    KoDrag.h
    KoDragOdfSaveHelper.h
    KoFlake.h
    KoFrameShape.h
    KoGuidesData.h
    KoGridData.h
    KoImageCollection.h
    KoImageData.h
    KoInputDevice.h
    KoInsets.h
    KoPathSegment.h
    KoPointerEvent.h
    KoRTree.h
    KoSelection.h
    KoShape.h
    KoShapeAnchor.h
    KoShapeStrokeModel.h
    KoToolSelection.h
    KoShapeContainer.h
    KoShapeContainerModel.h
    KoShapeContainerDefaultModel.h
    KoShapeController.h
    KoShapeBasedDocumentBase.h
    KoShapeFactoryBase.h
    KoShapeGroup.h
    KoShapeLayer.h
    KoShapePainter.h
    KoShapeLoadingContext.h
    KoShapeOdfSaveHelper.h
    KoLoadingShapeUpdater.h
    KoShapeConfigWidgetBase.h
    KoShapeManager.h
    KoShapeManagerPaintingStrategy.h
    KoShapeRegistry.h
    KoShapeSavingContext.h
    KoShapeUserData.h
    KoShapeApplicationData.h
    KoSharedSavingData.h
    KoTextShapeDataBase.h
    KoToolBase.h
    KoToolManager.h
    KoToolFactoryBase.h
    KoToolProxy.h
    KoToolRegistry.h
    KoFilterEffect.h
    KoFilterEffectStack.h
    KoFilterEffectFactoryBase.h
    KoFilterEffectRegistry.h
    KoFilterEffectConfigWidgetBase.h
    KoFilterEffectRenderContext.h
    KoFilterEffectLoadingContext.h
    commands/KoEventActionAddCommand.h
    commands/KoEventActionRemoveCommand.h
    commands/KoParameterToPathCommand.h
    commands/KoPathCombineCommand.h
    commands/KoPathControlPointMoveCommand.h
    commands/KoPathFillRuleCommand.h
    commands/KoPathPointInsertCommand.h
    commands/KoPathPointMergeCommand.h
    commands/KoPathPointMoveCommand.h
    commands/KoPathPointRemoveCommand.h
    commands/KoPathPointTypeCommand.h
    commands/KoPathReverseCommand.h
    commands/KoPathSegmentTypeCommand.h
    commands/KoShapeAlignCommand.h
    commands/KoShapeBackgroundCommand.h
    commands/KoShapeStrokeCommand.h
    commands/KoShapeCreateCommand.h
    commands/KoShapeDeleteCommand.h
    commands/KoShapeDistributeCommand.h
    commands/KoShapeGroupCommand.h
    commands/KoShapeMoveCommand.h
    commands/KoShapeRenameCommand.h
    commands/KoShapeReorderCommand.h
    commands/KoShapeShadowCommand.h
    commands/KoShapeShearCommand.h
    commands/KoShapeSizeCommand.h
    commands/KoShapeTransformCommand.h
    commands/KoShapeUngroupCommand.h
    commands/KoSubpathRemoveCommand.h
    commands/KoPathShapeMarkerCommand.h
    tools/KoInteractionTool.h
    tools/KoPanTool.h
    KoViewConverter.h
    DESTINATION
    ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel
)

endif()
