INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../visitors \
	-I$(srcdir)/../dockers \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../render \
	$(LIBFREETYPE_CFLAGS) $(all_includes)

noinst_LTLIBRARIES = libkarboncore.la

noinst_HEADERS = \
	vcanvas.h \
	vcolor.h \
	vcomposite.h \
	vdashpattern.h \
	vdocument.h \
	vfill.h \
	vglobal.h \
	vgradient.h \
	vgroup.h \
	vlayer.h \
	vobject.h \
	vpath.h \
	vpattern.h \
	vsegment.h \
	vselection.h \
	vstroke.h \
	vvisitor.h \
	vlayer_iface.h \
	vtext.h

libkarboncore_la_SOURCES = \
	vcanvas.cc \
	vcolor.cc \
	vcomposite.cc \
	vdashpattern.cc \
	vdocument.cc \
	vfill.cc \
	vgradient.cc \
	vgroup.cc \
	vlayer.cc \
	vobject.cc \
	vpath.cc \
	vpattern.cc \
	vsegment.cc \
	vselection.cc \
	vstroke.cc \
	vvisitor.cc \
	vlayer_iface.cc \
	vlayer_iface.skel \
	vtext.cc

libkarboncore_la_METASOURCES = \
	AUTO

libkarboncore_la_LDFLAGS = $(KDE_RPATH) $(LIBFREETYPE_RPATH)
libkarboncore_la_LIBADD = $(LIBFREETYPE_LIBS) -lfontconfig
