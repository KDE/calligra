kde_module_LTLIBRARIES = \
	libkarbonepsexport.la \
	libkarbonepsimport.la

libkarbonepsexport_la_SOURCES = \
	epsexport.cc \
	epsexportdlg.cc

libkarbonepsexport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonepsexport_la_LIBADD  = $(LIB_KOFFICEUI) ../../../karbon/libkarboncommon.la

libkarbonepsimport_la_SOURCES = epsimport.cc pscommentlexer.cc
libkarbonepsimport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonepsimport_la_LIBADD  = $(LIB_KOFFICEUI)

INCLUDES = \
	$(KOFFICE_INCLUDES) \
	$(KOPAINTER_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	-I$(top_srcdir)/karbon/visitors \
	$(all_includes)

service_DATA = \
	karbon_eps_export.desktop \
	karbon_eps_import.desktop \
	karbon_ps_import.desktop

servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	epsexport.h \
	epsexportdlg.h \
	epsimport.h \
	pscommentlexer.h

METASOURCES = AUTO
