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
	vdocument.h \
	vfill.h \
	vglobal.h \
	vgradient.h \
	vgroup.h \
	vlayer.h \
	vobject.h \
	vpath.h \
	vsegment.h \
	vsegmentlist.h \
	vselection.h \
	vstroke.h
#	vtext.h

libkarboncore_la_SOURCES = \
	vcanvas.cc \
	vcolor.cc \
	vdashpattern.cc \
	vdocument.cc \
	vfill.cc \
	vgradient.cc \
	vgroup.cc \
	vlayer.cc \
	vobject.cc \
	vpath.cc \
	vsegment.cc \
	vsegmentlist.cc \
	vselection.cc \
	vstroke.cc
#	vtext.cc

libkarboncore_la_METASOURCES = \
	AUTO

libkarboncore_la_LDFLAGS = $(KDE_RPATH) $(LIBFREETYPE_RPATH)
libkarboncore_la_LIBADD = $(LIBFREETYPE_LIBS)
