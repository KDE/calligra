

## KDE4 includes needed for kdemacros.h
include_directories( ${KDE4_INCLUDES} )


########### next target ###############

set(kdchart_LIB_SRCS 
    KDChartAbstractAreaBase.cpp
    KDChartAbstractArea.cpp
    KDChartAbstractAreaWidget.cpp
    KDChartAbstractAxis.cpp
    KDChartAbstractCartesianDiagram.cpp
    KDChartAbstractCoordinatePlane.cpp
    KDChartAbstractDiagram.cpp
    KDChartAbstractGrid.cpp
    KDChartAbstractPieDiagram.cpp
    KDChartAbstractPolarDiagram.cpp
    KDChartAbstractProxyModel.cpp
    KDChartAbstractThreeDAttributes.cpp
    KDChartAttributesModel.cpp
    KDChartBackgroundAttributes.cpp
    KDChartBarAttributes.cpp
    KDChartBarDiagram.cpp
    KDChartCartesianAxis.cpp
    KDChartCartesianCoordinatePlane.cpp
    KDChartCartesianGrid.cpp
    KDChartChart.cpp
    KDChartDatasetProxyModel.cpp
    KDChartDatasetSelector.cpp
    KDChartDataValueAttributes.cpp
    KDChartDiagramObserver.cpp
    KDChartFrameAttributes.cpp
    KDChartGridAttributes.cpp
    KDChartHeaderFooter.cpp
    KDChartLayoutItems.cpp
    KDChartLegend.cpp
    KDChartLineAttributes.cpp
    KDChartLineDiagram.cpp
    KDChartMarkerAttributes.cpp
    KDChartMeasure.cpp
    KDChartPaintContext.cpp
    KDChartPalette.cpp
    KDChartPieAttributes.cpp
    KDChartPieDiagram.cpp
    KDChartPolarCoordinatePlane.cpp
    KDChartPolarDiagram.cpp
    KDChartPolarGrid.cpp
    KDChartPosition.cpp
    KDChartRelativePosition.cpp
    KDChartRingDiagram.cpp
    KDChartTextArea.cpp
    KDChartTextAttributes.cpp
    KDChartThreeDBarAttributes.cpp
    KDChartThreeDLineAttributes.cpp
    KDChartThreeDPieAttributes.cpp
    KDChartWidget.cpp
    KDTextDocument.cpp
)

kde4_automoc(${kdchart_LIB_SRCS})

kde4_add_ui_files(kdchart_LIB_SRCS
    KDChartDatasetSelector.ui
)

kde4_add_library(kdchart SHARED ${kdchart_LIB_SRCS})

#target_link_libraries(kdchart ${QT_QT3SUPPORT_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${ZLIB_LIBRARY} )
target_link_libraries(kdchart  ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${ZLIB_LIBRARY} )

set_target_properties(kdchart PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kdchart  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############

#install( FILES KDChartTableBase.h KDChartListTable.h KDChartVectorTable.h KDChartTable.h KDChartParams.h KDFrameProfileSection.h KDChartPropertySet.h KDChartDataIntern.h KDChartDataRegion.h KDFrame.h KDChartGlobal.h KDChartWidget.h KDChartCustomBox.h KDChartEnums.h KDChartTextPiece.h KDChartAxisParams.h kdchart_export.h  DESTINATION ${INCLUDE_INSTALL_DIR})

install( FILES
    CartesianCoordinateTransformation.h
    KDChartAbstractAreaBase.h
    KDChartAbstractAreaBase_p.h
    KDChartAbstractArea.h
    KDChartAbstractArea_p.h
    KDChartAbstractAreaWidget.h
    KDChartAbstractAreaWidget_p.h
    KDChartAbstractAxis.h
    KDChartAbstractAxis_p.h
    KDChartAbstractCartesianDiagram.h
    KDChartAbstractCartesianDiagram_p.h
    KDChartAbstractCoordinatePlane.h
    KDChartAbstractCoordinatePlane_p.h
    KDChartAbstractDiagram.h
    KDChartAbstractDiagram_p.h
    KDChartAbstractGrid.h
    KDChartAbstractPieDiagram.h
    KDChartAbstractPieDiagram_p.h
    KDChartAbstractPolarDiagram.h
    KDChartAbstractPolarDiagram_p.h
    KDChartAbstractProxyModel.h
    KDChartAbstractThreeDAttributes.h
    KDChartAbstractThreeDAttributes_p.h
    KDChartAttributesModel.h
    KDChartBackgroundAttributes.h
    KDChartBarAttributes.h
    KDChartBarDiagram.h
    KDChartBarDiagram_p.h
    KDChartCartesianAxis.h
    KDChartCartesianAxis_p.h
    KDChartCartesianCoordinatePlane.h
    KDChartCartesianCoordinatePlane_p.h
    KDChartCartesianGrid.h
    KDChartChart.h
    KDChartChart_p.h
    KDChartDatasetProxyModel.h
    KDChartDatasetSelector.h
    KDChartDataValueAttributes.h
    KDChartDiagramObserver.h
    KDChartEnums.h
    kdchart_export.h
    KDChartFrameAttributes.h
    KDChartGlobal.h
    KDChartGridAttributes.h
    KDChartHeaderFooter.h
    KDChartHeaderFooter_p.h
    KDChartLayoutItems.h
    KDChartLegend.h
    KDChartLegend_p.h
    KDChartLineAttributes.h
    KDChartLineDiagram.h
    KDChartLineDiagram_p.h
    KDChartMarkerAttributes.h
    KDChartMeasure.h
    KDChartPaintContext.h
    KDChartPainterSaver_p.h
    KDChartPalette.h
    KDChartPieAttributes.h
    KDChartPieAttributes_p.h
    KDChartPieDiagram.h
    KDChartPieDiagram_p.h
    KDChartPolarCoordinatePlane.h
    KDChartPolarCoordinatePlane_p.h
    KDChartPolarDiagram.h
    KDChartPolarDiagram_p.h
    KDChartPolarGrid.h
    KDChartPosition.h
    KDChartRelativePosition.h
    KDChartRingDiagram.h
    KDChartRingDiagram_p.h
    KDChartTextArea.h
    KDChartTextArea_p.h
    KDChartTextAttributes.h
    KDChartThreeDBarAttributes.h
    KDChartThreeDBarAttributes_p.h
    KDChartThreeDLineAttributes.h
    KDChartThreeDLineAttributes_p.h
    KDChartThreeDPieAttributes.h
    KDChartThreeDPieAttributes_p.h
    KDChartWidget.h
    KDChartWidget_p.h
    KDChartZoomParameters.h
    KDTextDocument.h
    KDABLibFakes.h
DESTINATION ${INCLUDE_INSTALL_DIR})

