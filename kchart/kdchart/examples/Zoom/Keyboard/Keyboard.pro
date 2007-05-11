KDCHART_PATH = ../../..

include( $$KDCHART_PATH/examples/examples.pri )

TARGET = ZoomKeyboard

FORMS   = mainwindow.ui
SOURCES = main.cpp mainwindow.cpp zoomwidget.cpp
HEADERS = mainwindow.h zoomwidget.h

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
