KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES=$(KOPAINTER_INCLUDES) $(KOFFICEUI_INCLUDES) $(all_includes)

check_PROGRAMS = kopaintertest

noinst_HEADERS = koPainterTest.h 

METASOURCES = AUTO

kopaintertest_SOURCES = koPainterTest.cc
kopaintertest_LDFLAGS = $(all_libraries)
kopaintertest_LDADD = ../libkopainter.la
