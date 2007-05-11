KDCHART_PATH = ../..

include( $$KDCHART_PATH/g++.pri )

include( $$KDCHART_PATH/examples/examples.pri )

TARGET = DrawIntoPainter

HEADERS= mainwindow.h framewidget.h
SOURCES= main.cpp mainwindow.cpp framewidget.cpp
FORMS  = mainwindow.ui
RESOURCES = DrawIntoPainter.qrc

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
