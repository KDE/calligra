INCLUDES = $(KOFFICE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../tools \
	-I$(srcdir)/../render $(all_includes) $(LIBFREETYPE_CFLAGS)

noinst_LTLIBRARIES = libkarboncore.la

noinst_HEADERS = \
	vcanvas.h \
	vcolor.h \
	vfill.h \
	vglobal.h \
	vgradient.h \
	vlayer.h \
	vobject.h \
	vobjectlist.h \
	vpath.h \
	vpath_bounding.h \
	vpath_fill.h \
	vpath_intersect.h \
	vpath_stroke.h \
	vsegment.h \
	vsegmentlist.h \
	vsegment_tools.h \
	vstroke.h \
	vtext.h

libkarboncore_la_SOURCES = \
	vcanvas.cc \
	vcolor.cc \
	vfill.cc \
	vgradient.cc \
	vlayer.cc \
	vobject.cc \
	vobjectlist.cc \
	vpath.cc \
	vpath_bounding.cc \
	vpath_fill.cc \
	vpath_intersect.cc \
	vpath_stroke.cc \
	vsegment.cc \
	vsegmentlist.cc \
	vsegment_tools.cc \
	vstroke.cc \
	vtext.cc

libkarboncore_la_METASOURCES = \
	AUTO

libkarboncore_la_LIBADD = $(LIBFREETYPE_LIBS)
