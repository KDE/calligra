KDCHART_PATH = ../../..

TARGET = WidgetAdvanced

include( $$KDCHART_PATH/examples/examples.pri )

FORMS   = mainwindow.ui
SOURCES = main.cpp mainwindow.cpp
HEADERS = mainwindow.h

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
