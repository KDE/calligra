
INCLUDES= \
	-I$(srcdir) \
	$(KWMF_INCLUDES) \
	$(KOFFICE_INCLUDES) \
	$(KOPAINTER_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	-I$(top_srcdir)/karbon/commands \
	$(all_includes)

kde_module_LTLIBRARIES = libwmfimport.la libwmfexport.la

libwmfimport_la_LDFLAGS = -module $(KDE_PLUGIN)
libwmfimport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KOWMF) ../../../karbon/libkarboncommon.la
libwmfimport_la_SOURCES = wmfimport.cc wmfimportparser.cc

libwmfexport_la_LDFLAGS = -module $(KDE_PLUGIN)
libwmfexport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KOWMF) ../../../karbon/libkarboncommon.la
libwmfexport_la_SOURCES = wmfexport.cc

noinst_HEADERS = wmfimport.h wmfimportparser.h wmfexport.h 

METASOURCES = AUTO

service_DATA = karbon_wmf_import.desktop karbon_wmf_export.desktop
servicedir = $(kde_servicesdir)
