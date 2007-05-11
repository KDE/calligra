KDCHART_PATH = ../../..

include( $$KDCHART_PATH/examples/examples.pri )

TARGET = AxisLabels

HEADERS= mainwindow.h AdjustedCartesianAxis.h
SOURCES= main.cpp mainwindow.cpp AdjustedCartesianAxis.cpp
FORMS  = mainwindow.ui
RESOURCES = BarChart.qrc

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
