kde_module_LTLIBRARIES = libkarbonepsexport.la

libkarbonepsexport_la_LDFLAGS = $(KDE_PLUGIN) -module -avoid-version -no-undefined
libkarbonepsexport_la_LIBADD = \
	../../../../koffice/lib/kofficeui/libkofficeui.la

INCLUDES = \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../../../koffice/lib/kofficecore \
	-I$(srcdir)/../../../../koffice/lib/kofficeui \
	$(all_includes)

service_DATA = karbon_eps_export.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	epsexport.h

libkarbonepsexport_la_SOURCES = \
	epsexport.cc

libkarbonepsexport_la_METASOURCES = \
	AUTO

