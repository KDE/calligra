####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libasciiimport_la_LDFLAGS	= -module -avoid-version
libasciiimport_la_LIBADD = $(KOFFICE_LIBS)
libasciiexport_la_LDFLAGS	= -module -avoid-version
libasciiexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

lib_LTLIBRARIES = libasciiimport.la libasciiexport.la

libasciiimport_la_SOURCES = asciiimport.cc asciiimport_factory.cc
libasciiexport_la_SOURCES = asciiexport.cc asciiexport_factory.cc

noinst_HEADERS = asciiimport.h asciiimport_factory.h asciiexport.h \
asciiexport_factory.h

METASOURCES = AUTO

service_DATA = kword_ascii_import.desktop kword_ascii_export.desktop
servicedir = $(kde_servicesdir)
