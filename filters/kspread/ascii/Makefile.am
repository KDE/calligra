####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libkspreadasciiimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libkspreadasciiimport_la_LIBADD = $(KOFFICE_LIBS)
#libkspreadasciiexport_la_LDFLAGS	= -module -avoid-version -no-undefined
#libkspreadasciiexport_la_LIBADD = $(KOFFICE_LIBS)
####### Files

lib_LTLIBRARIES = libkspreadasciiimport.la

libkspreadasciiimport_la_SOURCES = asciiimport.cc asciiimport_factory.cc
#libkspreadasciiexport_la_SOURCES = asciiexport.cc asciiexport_factory.cc

noinst_HEADERS = asciiimport.h asciiimport_factory.h
# \ asciiexport.h asciiexport_factory.h
METASOURCES = AUTO

service_DATA = kspread_ascii_import.desktop
servicedir = $(kde_servicesdir)
