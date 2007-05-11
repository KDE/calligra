KDCHART_PATH = ../../..

include( $$KDCHART_PATH/examples/examples.pri )

HEADERS= mainwindow.h
SOURCES= main.cpp mainwindow.cpp
FORMS  = mainwindow.ui
RESOURCES = LineChart.qrc

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
