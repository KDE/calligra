kde_module_LTLIBRARIES = libkarbonsvgexport.la libkarbonsvgimport.la

libkarbonsvgexport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonsvgexport_la_LIBADD = \
	$(LIB_KOFFICEUI) \
	../../../karbon/libkarboncommon.la

libkarbonsvgimport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonsvgimport_la_LIBADD = \
	$(LIB_KOFFICEUI) \
	../../../karbon/libkarboncommon.la

INCLUDES = \
	$(KOFFICE_INCLUDES) \
	$(KOPAINTER_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	-I$(top_srcdir)/karbon/visitors \
	$(all_includes)

service_DATA = karbon_svg_export.desktop karbon_svg_import.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	svgexport.h

libkarbonsvgexport_la_SOURCES = \
	svgexport.cc

METASOURCES = AUTO

libkarbonsvgimport_la_SOURCES = \
	svgimport.cc

