####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
libwmlimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libwmlimport_la_LIBADD = $(KOFFICE_LIBS)
libwmlexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libwmlexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = libwmlimport.la libwmlexport.la

libwmlimport_la_SOURCES = wmlimport.cc wmlparser.cpp
libwmlexport_la_SOURCES = wmlexport.cc

noinst_HEADERS = wmlparser.h

METASOURCES = AUTO

service_DATA = kword_wml_import.desktop kword_wml_export.desktop
servicedir = $(kde_servicesdir)
