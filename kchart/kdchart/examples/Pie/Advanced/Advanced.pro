KDCHART_PATH = ../../..

TARGET = PieAdvanced

include( $$KDCHART_PATH/examples/examples.pri )

HEADERS= mainwindow.h
SOURCES= main.cpp mainwindow.cpp
FORMS  = mainwindow.ui
RESOURCES = Pie.qrc

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
