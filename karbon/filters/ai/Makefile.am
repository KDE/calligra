kde_module_LTLIBRARIES = libkarbonaiimport.la

libkarbonaiimport_la_LDFLAGS = $(KDE_PLUGIN) -module -avoid-version -no-undefined
libkarbonaiimport_la_LIBADD = \
	../../../../koffice/lib/kofficeui/libkofficeui.la

INCLUDES = \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../../../koffice/lib/kofficecore \
	-I$(srcdir)/../../../../koffice/lib/kofficeui \
	$(all_includes)

service_DATA = karbon_ai_import.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	aiimport.h

libkarbonaiimport_la_SOURCES = \
	aiimport.cc

libkarbonaiimport_la_METASOURCES = \
	AUTO

