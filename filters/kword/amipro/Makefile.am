####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
libamiproimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libamiproimport_la_LIBADD = $(KOFFICE_LIBS)

libamiproexport_la_LDFLAGS      = $(all_libraries) -module -avoid-version -no-undefined
libamiproexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = libamiproimport.la libamiproexport.la

libamiproimport_la_SOURCES = amiproimport.cc amiproparser.cpp
libamiproexport_la_SOURCES = amiproexport.cc

noinst_HEADERS = amiproparser.h

METASOURCES = AUTO

service_DATA = kword_amipro_import.desktop kword_amipro_export.desktop
servicedir = $(kde_servicesdir)
