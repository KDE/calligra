kde_module_LTLIBRARIES = libkarbonaiimport.la

libkarbonaiimport_la_LIBADD = \
	$(top_builddir)/lib/kofficeui/libkofficeui.la

libkarbonaiimport_la_LDFLAGS = \
	$(all_libraries) \
	$(KDE_RPATH) \
	-version-info 1:0:0 -no-undefined

INCLUDES = \
	-I$(srcdir)/.. \
        $(KOFFICE_INCLUDES) \
	$(all_includes)

service_DATA = karbon_ai_import.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	aicolor.h \
	aielement.h \
	ailexer.h \
	aiparserbase.h \
	karbonaiparserbase.h \
	aiimport.h 

libkarbonaiimport_la_SOURCES = \
	aicolor.cc \
	aielement.cc \
	ailexer.cc \
	aiparserbase.cc \
	karbonaiparserbase.cc \
	aiimport.cc 

libkarbonaiimport_la_METASOURCES = \
	AUTO

