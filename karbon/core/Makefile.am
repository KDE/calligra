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
	vdocument.h \
	vlayer.h \
	vobject.h \
	vobjectlist.h \
	vshape.h \
	vgroup.h \
	vpath.h \
	vsegment.h \
	vsegmentlist.h \
	vstroke.h
#	vtext.h

libkarboncore_la_SOURCES = \
	vcanvas.cc \
	vcolor.cc \
	vdashpattern.cc \
	vfill.cc \
	vgradient.cc \
	vdocument.cc \
	vlayer.cc \
	vobjectlist.cc \
	vshape.cc \
	vgroup.cc \
	vpath.cc \
	vsegment.cc \
	vsegmentlist.cc \
	vstroke.cc
#	vtext.cc

libkarboncore_la_METASOURCES = \
	AUTO

libkarboncore_la_LDFLAGS = $(KDE_RPATH) $(LIBFREETYPE_RPATH)
libkarboncore_la_LIBADD = $(LIBFREETYPE_LIBS)
