INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../visitors \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../render \
	$(LIBFREETYPE_CFLAGS) $(all_includes)

noinst_LTLIBRARIES = libkarboncore.la

noinst_HEADERS = \
	vcanvas.h \
	vcolor.h \
	vdashpattern.h \
	vfill.h \
	vglobal.h \
	vgradient.h \
	vlayer.h \
	vobject.h \
	vobjectlist.h \
	vgroup.h \
	vpath.h \
	vpath_bounding.h \
	vpath_intersect.h \
	vsegment.h \
	vsegmentlist.h \
	vsegmenttools.h \
	vstroke.h \
	vtext.h

libkarboncore_la_SOURCES = \
	vcanvas.cc \
	vcolor.cc \
	vdashpattern.cc \
	vfill.cc \
	vgradient.cc \
	vlayer.cc \
	vobject.cc \
	vobjectlist.cc \
	vgroup.cc \
	vpath.cc \
	vpath_bounding.cc \
	vpath_intersect.cc \
	vsegment.cc \
	vsegmentlist.cc \
	vsegmenttools.cc \
	vstroke.cc \
	vtext.cc

libkarboncore_la_METASOURCES = \
	AUTO

libkarboncore_la_LDFLAGS = $(KDE_RPATH) $(LIBFREETYPE_RPATH)
libkarboncore_la_LIBADD = $(LIBFREETYPE_LIBS)
