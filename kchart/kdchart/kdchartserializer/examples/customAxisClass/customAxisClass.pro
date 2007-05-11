KDCHART_PATH = ../../..

TARGET = customAxisClass

include( $$KDCHART_PATH/kdchartserializer/examples/examples.pri )

HEADERS= mainwindow.h \
         AdjustedCartesianAxis.h \
         AdjustedCartesianAxisSerializer.h

SOURCES= main.cpp \
         mainwindow.cpp \
         AdjustedCartesianAxis.cpp \
         AdjustedCartesianAxisSerializer.cpp

FORMS  = mainwindow.ui

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
