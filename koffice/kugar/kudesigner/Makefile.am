SUBDIRS = toolbar pics templates

INCLUDES = -I$(srcdir)/../kudesigner_lib -I$(srcdir)/../lib \
	$(KOPROPERTY_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes)

bin_PROGRAMS =

lib_LTLIBRARIES = libkudesignercore.la
libkudesignercore_la_SOURCES = kudesigner_view.cpp kudesigner_factory.cpp \
	kudesigner_doc.cpp
libkudesignercore_la_LDFLAGS = -avoid-version $(all_libraries) $(KDE_RPATH) -no-undefined
libkudesignercore_la_LIBADD = ../kudesigner_lib/libkudesignercommon.la $(LIB_KOFFICEUI) $(LIB_KPARTS) $(LIB_KOFFICECORE) $(LIB_KOPROPERTY) $(LIB_KIO) $(LIB_KDEPRINT)

## The part
kde_module_LTLIBRARIES = libkudesignerpart.la
libkudesignerpart_la_SOURCES = kudesigner_part.cpp
libkudesignerpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(KDE_RPATH)
libkudesignerpart_la_LIBADD = libkudesignercore.la $(LIB_KOFFICEUI) $(LIB_KPARTS) $(LIB_KOFFICECORE)

## The kdeinit loadable module
kdeinit_LTLIBRARIES = kudesigner.la
kudesigner_la_SOURCES = main.cpp
kudesigner_la_LDFLAGS = $(all_libraries) -module -avoid-version
kudesigner_la_LIBADD = $(LIB_KOFFICECORE)

appsdir = $(kde_appsdir)/Office
apps_DATA = kudesigner.desktop

METASOURCES = AUTO

rcdir = $(kde_datadir)/kudesigner
rc_DATA = kudesignerui.rc kudesigner_readonly.rc

EXTRA_DIST = \
	kudesigner_aboutdata.h \
	kudesigner_factory.h \
	kudesigner_part.h \
	kudesigner_view.h \
	$(rc_DATA) \
	$(apps_DATA)

KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKPROPERTY_EXPORT=

