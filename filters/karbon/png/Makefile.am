kde_module_LTLIBRARIES = libkarbonpngexport.la

libkarbonpngexport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonpngexport_la_LIBADD = \
	$(LIB_KOFFICEUI) \
	../../../karbon/libkarboncommon.la

INCLUDES = \
	$(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	-I$(top_srcdir)/karbon/render \
	-I$(top_srcdir)/karbon/visitors \
	$(all_includes)

service_DATA = karbon_png_export.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	pngexport.h

libkarbonpngexport_la_SOURCES = \
	pngexport.cc

libkarbonpngexport_la_METASOURCES = \
	AUTO

