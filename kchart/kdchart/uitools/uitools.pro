TEMPLATE = lib
TARGET = kdchartuitools
CONFIG(debug, debug|release) {
  !unix:TARGET = kdchartuitoolsd
}

include( ../variables.pri )

# can't do that, man:
# KDCHARTDIR = ..
KDCHARTBASE = ..

# Use the filename "kdchartd.dll" (or "kdchartd.lib") on Windows
# to avoid name clashes between debug/non-debug versions of the
# KD Chart library:
KDCHARTLIB = kdchart
CONFIG(debug, debug|release) {
    !unix: KDCHARTLIB = "kdchartd"
}
LIBS += -L$$KDCHARTBASE/lib -l$$KDCHARTLIB


DEFINES += UITOOLS_BUILD_UITOOLS_LIB

QT += xml 
CONFIG += designer

INCLUDEPATH+= $(QTDIR)/src/3rdparty/zlib \
              $$KDCHARTBASE/extra_include \
              $$KDCHARTBASE/src \
              .
DEPENDPATH += $(QTDIR)/src/3rdparty/zlib \
              .

UI_DIR = .

KDAB_EVAL{
  HEADERS += ../evaldialog/evaldialog.h
  SOURCES += ../evaldialog/evaldialog.cpp
  DEFINES += KDAB_EVAL
}


FORMS += KDChartLegendPropertiesWidget.ui \
         KDChartHeaderFooterPropertiesWidget.ui \
         KDChartAxisPropertiesWidget.ui \
         KDChartDiagramPropertiesWidget.ui \
         
HEADERS += KDChartLegendPropertiesWidget.h \
           KDChartLegendPropertiesWidget_p.h \
           KDChartAxisPropertiesWidget.h \
           KDChartAxisPropertiesWidget_p.h \
           KDChartHeaderFooterPropertiesWidget.h \
           KDChartHeaderFooterPropertiesWidget_p.h \
           KDChartWidgetSerializer.h \
           KDChartChartSerializer.h \
           KDChartDiagramPropertiesWidget.h \
           KDChartDiagramPropertiesWidget_p.h \ 
           
SOURCES += KDChartLegendPropertiesWidget.cpp \
           KDChartAxisPropertiesWidget.cpp \
           KDChartHeaderFooterPropertiesWidget.cpp \
           KDChartWidgetSerializer.cpp \
           KDChartChartSerializer.cpp \ 
           KDChartDiagramPropertiesWidget.cpp \
