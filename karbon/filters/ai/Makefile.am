kde_module_LTLIBRARIES = libkarbonaiimport.la

libkarbonaiimport_la_LIBADD = \
	$(srcdir)/../../../../koffice/lib/kofficeui/libkofficeui.la

libkarbonaiimport_la_LDFLAGS = \
	$(all_libraries) \
	$(KDE_RPATH) \
	-version-info 1:0:0 -no-undefined

INCLUDES = \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../../../koffice/lib/kofficecore \
	-I$(srcdir)/../../../../koffice/lib/kofficeui \
	$(all_includes)

service_DATA = karbon_ai_import.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	aidocument.h \
	aiimport.h 

libkarbonaiimport_la_SOURCES = \
	aidocument.cc \
	aiimport.cc 

libkarbonaiimport_la_METASOURCES = \
	AUTO

