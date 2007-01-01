

## KDE4 includes needed for kdemacros.h
include_directories( ${KDE4_INCLUDES} )


########### next target ###############

set(kdchart_LIB_SRCS 
   KDChart.cpp 
   KDChartAreaPainter.cpp 
   KDChartAxesPainter.cpp 
   KDChartAxisParams.cpp 
   KDChartBarPainter.cpp 
   KDChartBaseSeries.cpp 
   KDChartBWPainter.cpp 
   KDChartCustomBox.cpp 
   KDChartDataIntern.cpp 
   KDChartHiLoPainter.cpp 
   KDChartLinesPainter.cpp 
   KDChartPainter.cpp 
   KDChartParams.cpp 
   KDChartParams_frame.cpp 
   KDChartParams_io.cpp 
   KDChartPiePainter.cpp 
   KDChartPlaneSeries.cpp 
   KDChartPolarPainter.cpp 
   KDChartPropertySet.cpp 
   KDChartRingPainter.cpp 
   KDChartSeriesCollection.cpp 
   KDChartTableBase.cpp 
   KDChartTextPiece.cpp 
   KDChartVectorSeries.cpp 
   KDChartVectorTable.cpp 
   KDChartWidget.cpp 
   KDDrawText.cpp 
   KDFrame.cpp 
   KDFrameProfileSection.cpp 
   KDXMLTools.cpp 
   KDChartEnums.cpp )

kde4_automoc(${kdchart_LIB_SRCS})

kde4_add_library(kdchart SHARED ${kdchart_LIB_SRCS})

target_link_libraries(kdchart ${QT_QT3SUPPORT_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${ZLIB_LIBRARY} )

set_target_properties(kdchart PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kdchart  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############

install( FILES KDChartTableBase.h KDChartListTable.h KDChartVectorTable.h KDChartTable.h KDChartParams.h KDFrameProfileSection.h KDChartPropertySet.h KDChartDataIntern.h KDChartDataRegion.h KDFrame.h KDChartGlobal.h KDChartWidget.h KDChartCustomBox.h KDChartEnums.h KDChartTextPiece.h KDChartAxisParams.h kdchart_export.h  DESTINATION ${INCLUDE_INSTALL_DIR})

