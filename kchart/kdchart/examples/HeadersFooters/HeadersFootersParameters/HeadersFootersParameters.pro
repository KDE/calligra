KDCHART_PATH = ../../..

TARGET = HeadersFootersParameters

include( $$KDCHART_PATH/examples/examples.pri )

SOURCES= main.cpp

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
