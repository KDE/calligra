####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

libhtmlimport_la_LDFLAGS = -module -avoid-version -no-undefined

libhtmlimport_la_LIBADD = $(LIB_KHTML) $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libhtmlimport.la

libhtmlimport_la_SOURCES = htmlimport.cpp htmlimport_factory.cpp\
khtmlreader.cpp kwdwriter.cpp

noinst_HEADERS = htmlimport.h htmlimport_factory.h khtmlreader.h kwdwriter.h

METASOURCES = AUTO

service_DATA = kword_html_import.desktop
servicedir = $(kde_servicesdir)

