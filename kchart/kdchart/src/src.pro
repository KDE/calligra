# Subdir relative project main directory: ./src

# Target is a library:  kdchart
TEMPLATE = lib
# Use the filename "kdchartd.dll" (or "kdchartd.lib") on Windows
# to avoid name clashes between debug/non-debug versions of the
# KD Chart library:
TARGET = kdchart
CONFIG(debug, debug|release) {
    !unix: TARGET = kdchartd
}

include( ../variables.pri )

DEFINES += KDCHART_BUILD_KDCHART_LIB

FORMS += KDChartDatasetSelector.ui
HEADERS += KDChartGlobal.h \
           KDChartMeasure.h \
           CartesianCoordinateTransformation.h \
           KDChartAbstractCartesianDiagram.h \
           KDChartAbstractCartesianDiagram_p.h \
           KDChartAbstractCoordinatePlane.h \
           KDChartAbstractCoordinatePlane_p.h \
           KDChartCartesianCoordinatePlane.h \
           KDChartCartesianCoordinatePlane_p.h \
           KDChartPolarCoordinatePlane.h \
           KDChartPolarCoordinatePlane_p.h \
           KDChartChart.h \
           KDChartChart_p.h \
           KDChartWidget.h \
           KDChartWidget_p.h \
           KDChartAbstractDiagram.h \
           KDChartAbstractDiagram_p.h \
           KDChartAbstractPieDiagram.h \
           KDChartAbstractPieDiagram_p.h \
           KDChartAbstractPolarDiagram.h \
           KDChartAbstractPolarDiagram_p.h \
           KDChartAbstractAreaBase.h \
           KDChartAbstractAreaBase_p.h \
           KDChartAbstractArea.h \
           KDChartAbstractArea_p.h \
           KDChartTextArea.h \
           KDChartTextArea_p.h \
           KDChartAbstractAreaWidget.h \
           KDChartAbstractAreaWidget_p.h \
           KDChartAbstractAxis.h \
           KDChartAbstractProxyModel.h \
           KDChartAbstractGrid.h \
           KDChartCartesianGrid.h \
           KDChartPolarGrid.h \
           KDChartAttributesModel.h \
           KDChartBackgroundAttributes.h \
           KDChartBarAttributes.h \
           KDChartBarDiagram.h \
           KDChartBarDiagram_p.h \
           KDChartCartesianAxis.h \
           KDChartCartesianAxis_p.h \
           KDChartDatasetProxyModel.h \
           KDChartDatasetSelector.h \
           KDChartDataValueAttributes.h \
           KDChartDiagramObserver.h \
           KDChartEnums.h \
           KDChartFrameAttributes.h \
           KDChartGridAttributes.h \
           KDChartHeaderFooter.h \
           KDChartHeaderFooter_p.h \
           KDChartLayoutItems.h \
           KDChartLegend.h \
           KDChartLegend_p.h \
           KDChartLineAttributes.h \
           KDChartLineDiagram.h \
           KDChartLineDiagram_p.h \
           KDChartCartesianDiagramDataCompressor_p.h \
           KDChartNormalBarDiagram_p.h \
           KDChartNormalLineDiagram_p.h \
           KDChartStackedBarDiagram_p.h \
           KDChartStackedLineDiagram_p.h \
           KDChartPercentBarDiagram_p.h \
           KDChartPercentLineDiagram_p.h \
           KDChartMarkerAttributes.h \
           KDChartPaintContext.h \
           KDChartPalette.h \
           KDChartPieDiagram.h \
           KDChartPieDiagram_p.h \
           KDChartPolarDiagram.h \
           KDChartPolarDiagram_p.h \
           KDChartPosition.h \
           KDChartRelativePosition.h \
           KDChartRingDiagram.h \
           KDChartRingDiagram_p.h \
           KDChartTextAttributes.h \
           KDTextDocument.h \
           KDChartPieAttributes.h \
           KDChartPieAttributes_p.h \
           KDChartAbstractThreeDAttributes.h \
           KDChartAbstractThreeDAttributes_p.h \
           KDChartThreeDBarAttributes.h \
           KDChartThreeDBarAttributes_p.h \
           KDChartThreeDLineAttributes.h \
           KDChartThreeDLineAttributes_p.h \
           KDChartThreeDPieAttributes.h \
           KDChartThreeDPieAttributes_p.h \ 
           PrerenderedElements/KDChartTextLabelCache.h \
           # unused so far: KDChartSignalCompressor.h \
           Ternary/TernaryPoint.h \
           Ternary/TernaryConstants.h \
           Ternary/KDChartTernaryGrid.h \
           Ternary/KDChartTernaryCoordinatePlane.h \
           Ternary/KDChartTernaryCoordinatePlane_p.h \
           Ternary/KDChartTernaryAxis.h \
           Ternary/KDChartAbstractTernaryDiagram.h \
           Ternary/KDChartAbstractTernaryDiagram_p.h \
           Ternary/KDChartTernaryPointDiagram.h \
           Ternary/KDChartTernaryLineDiagram.h \
           Scenery/ChartGraphicsItem.h \
           Scenery/ReverseMapper.h \
           KDChartValueTrackerAttributes.h \
           KDChartPlotter.h \
           KDChartPlotter_p.h \
           KDChartNormalPlotter_p.h \

SOURCES += \
           KDChartMeasure.cpp \
           KDChartAbstractCartesianDiagram.cpp \
           KDChartAbstractCoordinatePlane.cpp \
           KDChartCartesianCoordinatePlane.cpp \
           KDChartPolarCoordinatePlane.cpp \
           KDChartChart.cpp \
           KDChartWidget.cpp \
           KDChartAbstractDiagram.cpp \
           KDChartAbstractPieDiagram.cpp \
           KDChartAbstractPolarDiagram.cpp \
           KDChartAbstractAreaBase.cpp \
           KDChartAbstractArea.cpp \
           KDChartTextArea.cpp \
           KDChartAbstractAreaWidget.cpp \
           KDChartAbstractAxis.cpp \
           KDChartAbstractProxyModel.cpp \
           KDChartAbstractGrid.cpp \
           KDChartCartesianGrid.cpp \
           KDChartPolarGrid.cpp \
           KDChartAttributesModel.cpp \
           KDChartBackgroundAttributes.cpp \
           KDChartBarAttributes.cpp \
           KDChartBarDiagram.cpp \
           KDChartBarDiagram_p.cpp \
           KDChartCartesianAxis.cpp \
           KDChartDatasetProxyModel.cpp \
           KDChartDatasetSelector.cpp \
           KDChartDataValueAttributes.cpp \
           KDChartDiagramObserver.cpp \
           KDChartFrameAttributes.cpp \
           KDChartGridAttributes.cpp \
           KDChartHeaderFooter.cpp \
           KDChartLayoutItems.cpp \
           KDChartLegend.cpp \
           KDChartLineAttributes.cpp \
           KDChartLineDiagram.cpp \
           KDChartLineDiagram_p.cpp \
           KDChartCartesianDiagramDataCompressor_p.cpp \
           KDChartNormalBarDiagram_p.cpp \
           KDChartNormalLineDiagram_p.cpp \
           KDChartStackedBarDiagram_p.cpp \
           KDChartStackedLineDiagram_p.cpp \
           KDChartPercentBarDiagram_p.cpp \
           KDChartPercentLineDiagram_p.cpp \
           KDChartMarkerAttributes.cpp \
           KDChartPaintContext.cpp \
           KDChartPalette.cpp \
           KDChartPieDiagram.cpp \
           KDChartPolarDiagram.cpp \
           KDChartPosition.cpp \
           KDChartRelativePosition.cpp \
           KDChartRingDiagram.cpp \
           KDTextDocument.cpp \
           KDChartTextAttributes.cpp \
           KDChartPieAttributes.cpp \
           KDChartAbstractThreeDAttributes.cpp \
           KDChartThreeDBarAttributes.cpp \
           KDChartThreeDLineAttributes.cpp \
           KDChartThreeDPieAttributes.cpp \
           PrerenderedElements/KDChartTextLabelCache.cpp \
           # unused so far: KDChartSignalCompressor.cpp \
           Ternary/KDChartTernaryAxis.cpp \
           Ternary/KDChartTernaryGrid.cpp \
           Ternary/TernaryPoint.cpp \
           Ternary/TernaryConstants.cpp \
           Ternary/KDChartTernaryCoordinatePlane.cpp \
           Ternary/KDChartAbstractTernaryDiagram.cpp \
           Ternary/KDChartTernaryPointDiagram.cpp \
           Ternary/KDChartTernaryLineDiagram.cpp \
           Scenery/ChartGraphicsItem.cpp \
           Scenery/ReverseMapper.cpp \
           KDChartValueTrackerAttributes.cpp \
           KDChartPlotter.cpp \
           KDChartPlotter_p.cpp \
           KDChartNormalPlotter_p.cpp \

CONFIG += warn-on

# We want users of kdchart to be able to use the lib without interference with Qt-specific keywords, e.g. "signals" that collides with Boost's Signals
DEFINES += QT_NO_KEYWORDS
DEFINES += emit=""

LIBFAKES_PATH = ../kdablibfakes

DEPENDPATH = ../include \
            $$LIBFAKES_PATH/include \
            $(QTDIR)/src/3rdparty/zlib \
            .
INCLUDEPATH = ../include \
              $$LIBFAKES_PATH/include \
              $(QTDIR)/src/3rdparty/zlib \
              .
linux-*{
  version_script{
    QMAKE_LFLAGS += -Wl,--version-script=libkdchart.map
    TARGETDEPS += libkdchart.map
  }
}

solaris-*{
LIBS *= -lsunmath
}

qsa{
  message(compiling QSA support into KD Chart)
  SOURCES += KDChartObjectFactory.cpp \
  KDChartWrapperFactory.cpp \
  wrappers/KDChartAxisParamsWrapper.cpp \
  wrappers/KDChartParamsWrapper.cpp \
  wrappers/KDChartCustomBoxWrapper.cpp
  HEADERS += KDChartObjectFactory.h \
  KDChartWrapperFactory.h \
  wrappers/KDChartAxisParamsWrapper.h \
  wrappers/KDChartParamsWrapper.h \
  wrappers/KDChartCustomBoxWrapper.h \
  factories/QtFactory.h \
  factories/QFontFactory.h
}
KDAB_EVAL{
  HEADERS += ../evaldialog/evaldialog.h
  SOURCES += ../evaldialog/evaldialog.cpp
  DEFINES += KDAB_EVAL
}
#*g++*{
#  QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
#}

