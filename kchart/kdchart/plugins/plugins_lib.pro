TEMPLATE = lib
TARGET = kdchartpluginlib
CONFIG(debug, debug|release) {
  !unix:TARGET = kdchartpluginlibd
}

include( ../variables.pri )

DEFINES += KDCHART_BUILD_PLUGIN_LIB
CONFIG += designer

# Remove when all strings have been surrounded by tr() or QLatin1String()
DEFINES -= QT_NO_CAST_FROM_ASCII

KDCHARTDIR = ../


# Use the filename "kdchartd.dll" (or "kdchartd.lib") on Windows
# to avoid name clashes between debug/non-debug versions of the
# KD Chart library:
KDCHARTLIB = kdchart
CONFIG(debug, debug|release) {
    !unix: KDCHARTLIB = "kdchartd"
}
LIBS += -L$$KDCHARTDIR/lib -l$$KDCHARTLIB


unix {
  LIBS += -lkdchartuitools
} else {
  CONFIG(debug, debug|release) {
    LIBS += -lkdchartuitoolsd
  } else {
    LIBS += -lkdchartuitools
  }
}
INCLUDEPATH +=  $$KDCHARTDIR/uitools \
                $$KDCHARTDIR/include \
                $$KDCHARTDIR/src \
                $$KDCHARTDIR/extra_include

DEPENDPATH += $$KDCHARTDIR/uitools

HEADERS = \
	kdchartdesignerextension.h \
	kdchartwidgetdesignercustomeditor.h \ 
        kdchartchartdesignercustomeditor.h \
        kdchartextrainfoextension.h \
        #diagramtype.h \

SOURCES = \
	kdchartdesignerextension.cpp \
	kdchartwidgetdesignercustomeditor.cpp \ 
        kdchartchartdesignercustomeditor.cpp \
        kdchartextrainfoextension.cpp \
        #diagramtype.cpp \

FORMS = \
        kdchartwidgetdesignercustomeditor.ui \
        kdchartchartdesignercustomeditor.ui \
        #diagramtype.ui \

