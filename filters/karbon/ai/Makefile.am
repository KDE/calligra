kde_module_LTLIBRARIES = libkarbonaiimport.la

libkarbonaiimport_la_LIBADD = \
	$(LIB_KOFFICEUI) \
	../../../karbon/libkarbonpart.la

libkarbonaiimport_la_LDFLAGS = \
	$(all_libraries) \
	$(KDE_RPATH) \
	-version-info 1:0:0 -no-undefined

INCLUDES = \
	$(KOFFICE_INCLUDES) \
	$(KOPAINTER_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	$(all_includes)

service_DATA = karbon_ai_import.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	aicolor.h \
	aielement.h \
	aiimport.h \
	ailexer.h \
	aiparserbase.h \
	karbonaiparserbase.h

libkarbonaiimport_la_SOURCES = \
	aicolor.cc \
	aielement.cc \
	aiimport.cc \
	ailexer.cc \
	aiparserbase.cc \
	karbonaiparserbase.cc

libkarbonaiimport_la_METASOURCES = \
	AUTO

