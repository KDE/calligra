INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	$(all_includes)

noinst_LTLIBRARIES = libkarbonshapes.la

noinst_HEADERS = \
	vellipse.h \
	vpolygon.h \
	vrectangle.h \
	vroundrect.h \
	vsinus.h \
	vspiral.h \
	vstar.h

libkarbonshapes_la_SOURCES = \
	vellipse.cc \
	vpolygon.cc \
	vrectangle.cc \
	vroundrect.cc \
	vsinus.cc \
	vspiral.cc \
	vstar.cc

libkarbonshapes_la_METASOURCES = \
	AUTO

