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
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/visitors \
	$(all_includes)

service_DATA = karbon_ai_import.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	aicolor.h \
	aielement.h \
	ailexer.h \
	aiparserbase.h \
	karbonaiparserbase.h \
	vdocument.h \
	aiimport.h

libkarbonaiimport_la_SOURCES = \
	aicolor.cc \
	aielement.cc \
	ailexer.cc \
	aiparserbase.cc \
	karbonaiparserbase.cc \
	vdocument.cc \
	aiimport.cc

libkarbonaiimport_la_METASOURCES = \
	AUTO

