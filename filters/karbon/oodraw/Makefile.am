####### General stuff

INCLUDES= -I$(srcdir)/../../liboofilter \
	$(KOFFICE_INCLUDES) \
	$(KOPAINTER_INCLUDES) \
	-I$(top_srcdir)/karbon \
	-I$(top_srcdir)/karbon/core \
	$(all_includes)

####### Files
kde_module_LTLIBRARIES = liboodrawimport.la


liboodrawimport_la_SOURCES = oodrawimport.cc
liboodrawimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liboodrawimport_la_LIBADD  = ../../liboofilter/liboofilter.la \
							../../../karbon/libkarboncommon.la \
							$(KOFFICE_LIBS)

METASOURCES = AUTO

service_DATA = karbon_oodraw_import.desktop

servicedir = $(kde_servicesdir)
