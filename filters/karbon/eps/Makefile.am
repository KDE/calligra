kde_module_LTLIBRARIES = libkarbonepsexport.la

libkarbonepsexport_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonepsexport_la_LIBADD = $(LIB_KOFFICEUI)

INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	$(all_includes)

service_DATA = karbon_eps_export.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	epsexport.h

libkarbonepsexport_la_SOURCES = \
	epsexport.cc

libkarbonepsexport_la_METASOURCES = \
	AUTO

