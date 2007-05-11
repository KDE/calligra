KDCHART_PATH = ../../..

TARGET = PolarAdvanced

include( $$KDCHART_PATH/examples/examples.pri )

HEADERS= mainwindow.h
SOURCES= main.cpp mainwindow.cpp
FORMS  = mainwindow.ui

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
