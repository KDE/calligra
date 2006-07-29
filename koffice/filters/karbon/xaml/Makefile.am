kde_module_LTLIBRARIES = libkarbonxamlexport.la libkarbonxamlimport.la

libkarbonxamlexport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonxamlexport_la_LIBADD = \
	$(LIB_KOFFICEUI) \
	../../../karbon/libkarboncommon.la

libkarbonxamlimport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonxamlimport_la_LIBADD = \
	$(LIB_KOFFICEUI) \
	../../../karbon/libkarboncommon.la

INCLUDES = \
	$(KOFFICE_INCLUDES) \
	$(KOPAINTER_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	-I$(top_srcdir)/karbon/visitors \
	$(all_includes)

service_DATA = karbon_xaml_export.desktop karbon_xaml_import.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	xamlexport.h \
	xamlimport.h

libkarbonxamlexport_la_SOURCES = \
	xamlexport.cc

libkarbonxamlimport_la_SOURCES = \
	xamlimport.cc

METASOURCES = AUTO

