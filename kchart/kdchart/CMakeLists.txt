

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





#original Makefile.am contents follow:

#lib_LTLIBRARIES = libkdchart.la
#
#libkdchart_la_SOURCES = KDChart.cpp 		\
#			KDChartAreaPainter.cpp	\
#			KDChartAxesPainter.cpp	\
#			KDChartAxisParams.cpp	\
#			KDChartBarPainter.cpp	\
#			KDChartBaseSeries.cpp \
#			KDChartBWPainter.cpp	\
#			KDChartCustomBox.cpp 	\
#			KDChartDataIntern.cpp 	\
#			KDChartHiLoPainter.cpp	\
#			KDChartLinesPainter.cpp	\
#			KDChartPainter.cpp	\
#			KDChartParams.cpp	\
#			KDChartParams_frame.cpp\
#			KDChartParams_io.cpp	\
#			KDChartPiePainter.cpp	\
#			KDChartPlaneSeries.cpp \
#			KDChartPolarPainter.cpp	\
#			KDChartPropertySet.cpp	\
#			KDChartRingPainter.cpp	\
#			KDChartSeriesCollection.cpp \
#			KDChartTableBase.cpp \
#			KDChartTextPiece.cpp	\
#			KDChartVectorSeries.cpp \
#			KDChartVectorTable.cpp \
#			KDChartWidget.cpp 	\
#			KDDrawText.cpp		\
#			KDFrame.cpp 		\
#			KDFrameProfileSection.cpp \
#			KDXMLTools.cpp          \
#                        KDChartEnums.cpp
#
#libkdchart_la_LDFLAGS = $(all_libraries) -no-undefined
#libkdchart_la_LIBADD = $(LIB_QT) $(LIB_KDECORE)
#
#noinst_HEADERS =	KDChart.h 		\
#			KDChartAreaPainter.h	\
#			KDChartAxesPainter.h	\
#			KDChartBarPainter.h	\
#			KDChartBaseSeries.h \
#			KDChartBWPainter.h	\
#			KDChartDataIntern.h \
#			KDChartDataRegion.h \
#			KDChartHiLoPainter.h	\
#			KDChartLinesPainter.h	\
#			KDChartNotEnoughSpaceException.h	\
#			KDChartUnknownTypeException.h \
#			KDChartPainter.h	\
#			KDChartPiePainter.h	\
#			KDChartPlaneSeries.h \
#			KDChartPolarPainter.h	\
#			KDChartRingPainter.h	\
#			KDChartSeriesCollection.h \
#			KDChartTextPiece.h	\
#			KDChartUnknownTypeException.h	\
#			KDChartVectorSeries.h \
#			KDDrawText.h 		\
#			KDXMLTools.h
#METASOURCES = AUTO
#
#INCLUDES  = $(KOFFICECORE_INCLUDES) -I$(srcdir)/.. -I$(srcdir) $(all_includes)
#
#include_HEADERS =	KDChartTableBase.h	\
#			KDChartListTable.h	\
#			KDChartVectorTable.h	\
#			KDChartTable.h		\
#			KDChartParams.h		\
#			KDFrameProfileSection.h	\
#			KDChartPropertySet.h	\
#			KDChartDataIntern.h	\
#			KDChartDataRegion.h	\
#			KDFrame.h		\
#			KDChartGlobal.h		\
#			KDChartWidget.h		\
#			KDChartCustomBox.h	\
#			KDChartEnums.h		\
#			KDChartTextPiece.h	\
#			KDChartAxisParams.h \
#			kdchart_export.h
#
