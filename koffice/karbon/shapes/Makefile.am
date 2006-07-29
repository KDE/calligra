INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	$(all_includes)

noinst_LTLIBRARIES = libkarbonshapes.la

noinst_HEADERS = \
	vellipse.h \
	vpolyline.h \
	vpolygon.h \
	vrectangle.h \
	vsinus.h \
	vspiral.h \
	vstar.h

libkarbonshapes_la_SOURCES = \
	vellipse.cc \
	vpolyline.cc \
	vpolygon.cc \
	vrectangle.cc \
	vsinus.cc \
	vspiral.cc \
	vstar.cc

libkarbonshapes_la_METASOURCES = \
	AUTO

