KDCHART_PATH = ../../..

TARGET = complexLayout

include( $$KDCHART_PATH/kdchartserializer/examples/examples.pri )

HEADERS= mainwindow.h
SOURCES= main.cpp mainwindow.cpp
FORMS  = mainwindow.ui

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
