KDCHART_PATH = ../../..

include( $$KDCHART_PATH/examples/examples.pri )

TARGET = ZoomScrollBars

HEADERS= mainwindow.h
SOURCES= main.cpp mainwindow.cpp
FORMS  = mainwindow.ui
RESOURCES = ZoomScrollBars.qrc

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
