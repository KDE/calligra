KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES=$(KOPAINTER_INCLUDES) $(KOFFICEUI_INCLUDES) $(all_includes)

check_PROGRAMS = koPainterTest

noinst_HEADERS = koPainterTest.h 

METASOURCES = AUTO

koPainterTest_SOURCES = koPainterTest.cc
koPainterTest_LDFLAGS = $(all_libraries)
koPainterTest_LDADD = ../libkopainter.la
