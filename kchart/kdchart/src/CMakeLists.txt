

## KDE4 includes needed for kdemacros.h
include_directories(
        ${KDE4_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}/../include 
	${CMAKE_CURRENT_SOURCE_DIR}/../kdablibfakes/include 
)

ADD_DEFINITIONS(-DKDCHART_BUILD_KDCHART_LIB)

########### next target ###############

set(kdchart_LIB_SRCS 
    KDChartMeasure.cpp
    KDChartAbstractCartesianDiagram.cpp
    KDChartAbstractCoordinatePlane.cpp
    KDChartCartesianCoordinatePlane.cpp
    KDChartPolarCoordinatePlane.cpp
    KDChartChart.cpp
    KDChartWidget.cpp
    KDChartAbstractDiagram.cpp
    KDChartAbstractPieDiagram.cpp
    KDChartAbstractPolarDiagram.cpp
    KDChartAbstractAreaBase.cpp
    KDChartAbstractArea.cpp
    KDChartTextArea.cpp
    KDChartAbstractAreaWidget.cpp
    KDChartAbstractAxis.cpp
    KDChartAbstractProxyModel.cpp
    KDChartAbstractGrid.cpp
    KDChartCartesianGrid.cpp
    KDChartPolarGrid.cpp
    KDChartAttributesModel.cpp
    KDChartBackgroundAttributes.cpp
    KDChartBarAttributes.cpp
    KDChartBarDiagram.cpp
    KDChartCartesianAxis.cpp
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
    KDChartPaintContext.cpp
    KDChartPalette.cpp
    KDChartPieDiagram.cpp
    KDChartPolarDiagram.cpp
    KDChartPosition.cpp
    KDChartRelativePosition.cpp
    KDChartRingDiagram.cpp
    KDTextDocument.cpp
    KDChartTextAttributes.cpp
    KDChartPieAttributes.cpp
    KDChartAbstractThreeDAttributes.cpp
    KDChartThreeDBarAttributes.cpp
    KDChartThreeDLineAttributes.cpp     
    KDChartThreeDPieAttributes.cpp
    KDChartSignalCompressor.cpp
    KDChartBarDiagram_p.cpp
    KDChartCartesianDiagramDataCompressor_p.cpp
    KDChartLineDiagram_p.cpp
    KDChartNormalBarDiagram_p.cpp
    KDChartNormalLineDiagram_p.cpp
    KDChartNormalPlotter_p.cpp
    KDChartPercentBarDiagram_p.cpp
    KDChartPercentLineDiagram_p.cpp
    KDChartPlotter.cpp
    KDChartPlotter_p.cpp
    KDChartStackedBarDiagram_p.cpp
    KDChartStackedLineDiagram_p.cpp
    KDChartValueTrackerAttributes.cpp
    PrerenderedElements/KDChartTextLabelCache.cpp
    Scenery/ReverseMapper.cpp
    Scenery/ChartGraphicsItem.cpp
)

qt4_wrap_ui(kdchart_LIB_SRCS
    KDChartDatasetSelector.ui
)

kde4_add_library(kdchart SHARED ${kdchart_LIB_SRCS})

#target_link_libraries(kdchart ${QT_QT3SUPPORT_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${ZLIB_LIBRARY} )
target_link_libraries(kdchart  ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${ZLIB_LIBRARY} )

set_target_properties(kdchart PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kdchart  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############


#NOTHING
