kde_module_LTLIBRARIES = libkarbonxcfexport.la

libkarbonxcfexport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonxcfexport_la_LIBADD = \
	$(LIB_KOFFICEUI) \
	../../../karbon/libkarboncommon.la

INCLUDES = \
	$(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	-I$(top_srcdir)/karbon/render \
	-I$(top_srcdir)/karbon/visitors \
	$(all_includes)

service_DATA = karbon_xcf_export.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	xcfexport.h

libkarbonxcfexport_la_SOURCES = \
	xcfexport.cc

libkarbonxcfexport_la_METASOURCES = \
	AUTO

