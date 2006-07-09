####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
libdocbookexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libdocbookexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = libdocbookexport.la

libdocbookexport_la_SOURCES = docbookexport.cc

noinst_HEADERS = docbookexport.h 

METASOURCES = AUTO

service_DATA = kword_docbook_export.desktop
servicedir = $(kde_servicesdir)
