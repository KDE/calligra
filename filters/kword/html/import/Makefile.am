####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)

libhtmlimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined

libhtmlimport_la_LIBADD = $(LIB_KHTML) $(KOFFICE_LIBS) $(LIB_KOTEXT)

####### Files

kde_module_LTLIBRARIES = libhtmlimport.la

libhtmlimport_la_SOURCES = htmlimport.cpp khtmlreader.cpp kwdwriter.cpp

noinst_HEADERS = htmlimport.h khtmlreader.h kwdwriter.h

METASOURCES = AUTO

service_DATA = kword_html_import.desktop
servicedir = $(kde_servicesdir)
