####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libdocbookexport_la_LDFLAGS	= -module -avoid-version -no-undefined
libdocbookexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

lib_LTLIBRARIES = libdocbookexport.la

libdocbookexport_la_SOURCES = docbookexport.cc docbookexport_factory.cc

noinst_HEADERS = docbookexport.h docbookexport_factory.h

METASOURCES = AUTO

service_DATA = kword_docbook_export.desktop
servicedir = $(kde_servicesdir)
