INCLUDES = $(KOFFICE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../tools \
	-I$(srcdir)/../render $(LIBFREETYPE_CFLAGS) $(all_includes)

noinst_LTLIBRARIES = libkarbonvisitors.la

noinst_HEADERS = \
	vvisitor.h

libkarbonvisitors_la_SOURCES =

libkarbonvisitors_la_METASOURCES = \
	AUTO

libkarbonvisitors_la_LDFLAGS = $(KDE_RPATH) $(LIBFREETYPE_RPATH)
libkarbonvisitors_la_LIBADD = $(LIBFREETYPE_LIBS)
