INCLUDES = \
	$(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../render \
	$(LIBFREETYPE_CFLAGS) $(all_includes)

noinst_LTLIBRARIES = libkarbonvisitors.la

noinst_HEADERS = \
	vboolean.h \
	vcleanup.h \
	vflatten.h \
	vinsertknots.h \
	vroundcorners.h \
	vvisitor.h \
	vwhirlpinch.h

libkarbonvisitors_la_SOURCES = \
	vboolean.cc \
	vcleanup.cc \
	vflatten.cc \
	vinsertknots.cc \
	vroundcorners.cc \
	vvisitor.cc \
	vwhirlpinch.cc

libkarbonvisitors_la_METASOURCES = \
	AUTO

libkarbonvisitors_la_LDFLAGS = $(KDE_RPATH) $(LIBFREETYPE_RPATH)
libkarbonvisitors_la_LIBADD = $(LIBFREETYPE_LIBS)
