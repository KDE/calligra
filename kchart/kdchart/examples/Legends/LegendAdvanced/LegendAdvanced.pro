KDCHART_PATH = ../../..

include( $$KDCHART_PATH/examples/examples.pri )

HEADERS= mainwindow.h \
         derivedaddlegenddialog.h
SOURCES= main.cpp \
         mainwindow.cpp \
         derivedaddlegenddialog.cpp
FORMS  = mainwindow.ui \
         addlegenddialog.ui
RESOURCES = Legends.qrc

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
