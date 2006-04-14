kde4_header()




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

target_link_libraries(kdchart ${KDE4_KDECORE_LIBS} ${QT_QTXML_LIBRARY} )

set_target_properties(kdchart PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kdchart )


########### install files ###############

install_files( /include FILES KDChartTableBase.h KDChartListTable.h KDChartVectorTable.h KDChartTable.h KDChartParams.h KDFrameProfileSection.h KDChartPropertySet.h KDChartDataIntern.h KDChartDataRegion.h KDFrame.h KDChartGlobal.h KDChartWidget.h KDChartCustomBox.h KDChartEnums.h KDChartTextPiece.h KDChartAxisParams.h kdchart_export.h )

kde4_footer()



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
