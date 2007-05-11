KDCHART_PATH = ../../..

include( $$KDCHART_PATH/examples/examples.pri )

TARGET = AxisParameters

HEADERS= mainwindow.h
SOURCES= main.cpp mainwindow.cpp
FORMS  = mainwindow.ui
RESOURCES = Axes.qrc

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
