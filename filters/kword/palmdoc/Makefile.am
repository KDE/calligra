####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
libpalmdocimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libpalmdocimport_la_LIBADD = $(KOFFICE_LIBS)
libpalmdocexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libpalmdocexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = libpalmdocimport.la libpalmdocexport.la

libpalmdocimport_la_SOURCES = palmdocimport.cc palmdb.cpp palmdoc.cpp
libpalmdocexport_la_SOURCES = palmdocexport.cc palmdb.cpp palmdoc.cpp

noinst_HEADERS = palmdocimport.h palmdb.h palmdoc.h

METASOURCES = AUTO

service_DATA = kword_palmdoc_import.desktop kword_palmdoc_export.desktop
servicedir = $(kde_servicesdir)
