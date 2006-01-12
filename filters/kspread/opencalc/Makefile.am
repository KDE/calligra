####### General stuff

INCLUDES= -I$(srcdir)/../../liboofilter -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = libopencalcimport.la libopencalcexport.la

libopencalcexport_la_SOURCES = opencalcexport.cc opencalcstyleexport.cc
libopencalcexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libopencalcexport_la_LIBADD  = ../../../kspread/libkspreadcommon.la ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)

libopencalcimport_la_SOURCES = opencalcimport.cc
libopencalcimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libopencalcimport_la_LIBADD  = ../../../kspread/libkspreadcommon.la ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)

METASOURCES = AUTO

service_DATA = kspread_opencalc_import.desktop kspread_opencalc_export.desktop

servicedir = $(kde_servicesdir)
