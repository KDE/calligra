INCLUDES = -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = libkugarpart.la
libkugarpart_la_SOURCES = kugar_part.cpp kugar_factory.cpp kugar_view.cpp
libkugarpart_la_LDFLAGS = $(all_libraries) -module -avoid-version
libkugarpart_la_LIBADD = ../lib/libkugar.la $(LIB_KPARTS) $(LIB_KDEPRINT) $(LIB_KOFFICECORE)

libkugarpart_la_METASOURCES = AUTO

appsdir = $(kde_appsdir)/Office
apps_DATA = kugar.desktop

rcdir = $(kde_datadir)/kugar
rc_DATA = kugarpart.rc

dtddir = $(kde_datadir)/kugar/dtd
dtd_DATA = kugartemplate.dtd

EXTRA_DIST = \
	kugar_part.h \
	$(apps_DATA) \
	$(rc_DATA) \
	$(dtd_DATA)

CLEANFILES = kugarpart_meta_unload.cpp


## The kdeinit loadable module
lib_LTLIBRARIES = kugar.la
kugar_la_SOURCES = main.cpp
kugar_la_LDFLAGS = $(all_libraries) -module -avoid-version
kugar_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kugar
kugar_SOURCES = dummy.cc
kugar_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kugar_LDADD = kugar.la

dummy.cc:
	echo > dummy.cc
kde_services_DATA = kugarpart.desktop
