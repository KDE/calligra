INCLUDES = -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)

lib_LTLIBRARIES = libkugarpart.la
libkugarpart_la_SOURCES = kugar_part.cpp
libkugarpart_la_LDFLAGS = $(all_libraries) -module -avoid-version
libkugarpart_la_LIBADD = ../lib/libkugar.la $(LIB_KPARTS)

libkugarpart_la_METASOURCES = AUTO

appsdir = $(kde_appsdir)/Office
apps_DATA = kugar.desktop

rcdir = $(kde_datadir)/kugar
rc_DATA = kugarpart.rc

mimedir = $(kde_mimedir)/application
mime_DATA = x-kugar.desktop

dtddir = $(kde_datadir)/kugar/dtd
dtd_DATA = kugartemplate.dtd

EXTRA_DIST = \
	kugar_part.h \
	$(apps_DATA) \
	$(rc_DATA) \
	$(mime_DATA) \
	$(dtd_DATA)

CLEANFILES = kugarpart_meta_unload.cpp
