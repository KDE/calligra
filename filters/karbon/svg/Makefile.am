kde_module_LTLIBRARIES = libkarbonsvgexport.la

libkarbonsvgexport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonsvgexport_la_LIBADD = \
	$(LIB_KOFFICEUI) \
	../../../karbon/libkarbonpart.la

INCLUDES = \
	$(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	-I$(top_srcdir)/karbon/visitors \
	$(all_includes)

service_DATA = karbon_svg_export.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	svgexport.h

libkarbonsvgexport_la_SOURCES = \
	svgexport.cc

libkarbonsvgexport_la_METASOURCES = \
	AUTO

