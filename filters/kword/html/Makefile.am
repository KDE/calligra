####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libhtmlimport_la_LDFLAGS = -module -avoid-version 
libhtmlexport_la_LDFLAGS = -module -avoid-version

####### Files

lib_LTLIBRARIES = libhtmlimport.la libhtmlexport.la

libhtmlimport_la_SOURCES = htmlimport.cc htmlimport_factory.cc
libhtmlexport_la_SOURCES = htmlexport.cc htmlexport_factory.cc \
kword_xml2html.cc xmlparse.cc

noinst_HEADERS = htmlimport.h htmlimport_factory.h htmlexport.h \
htmlexport_factory.h kword_xml2html.h

METASOURCES = AUTO

service_DATA = kword_html_import.desktop kword_html_export.desktop
servicedir = $(kde_servicesdir)
