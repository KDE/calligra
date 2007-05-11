TEMPLATE = lib
CONFIG += plugin
TARGET = kdchart_plugin
CONFIG(debug, debug|release) {
    !unix:TARGET = kdchart_plugind
}


KDCHARTDIR = ../

include( $$KDCHARTDIR/variables.pri )

DEFINES += PLUGINS_PLUGIN_DLL


# Remove when all strings have been surrounded by tr() or QLatin1String()
DEFINES -= QT_NO_CAST_FROM_ASCII

CONFIG += designer debug_and_release

load(kdchart)

staticlib {
  CONFIG += kdchart_static
  CONFIG -= staticlib
}

HEADERS += plugins.h
SOURCES += plugins.cpp

unix {
  LIBS += -lkdchartpluginlib -lkdchartuitools
  kdchart_static:LIBS += -lkdchartuitools
} else {
  CONFIG(debug, debug|release) {
    LIBS += -lkdchartpluginlibd
    kdchart_static:LIBS += -lkdchartuitoolsd
  } else {
    LIBS += -lkdchartpluginlib
    kdchart_static:LIBS += -lkdchartuitools
  }
}

INCLUDEPATH += $$KDCHARTDIR/src \
               $$KDCHARTDIR/extra_include
DEPENDPATH +=  $$KDCHARTDIR/src

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
