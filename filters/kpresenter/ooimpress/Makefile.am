####### General stuff

INCLUDES= -I$(srcdir)/../../liboofilter $(KOFFICE_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = libooimpressimport.la libooimpressexport.la


libooimpressimport_la_SOURCES = ooimpressimport.cc
libooimpressimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libooimpressimport_la_LIBADD  = ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)

libooimpressexport_la_SOURCES = ooimpressexport.cc stylefactory.cc
libooimpressexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libooimpressexport_la_LIBADD  = ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)

METASOURCES = AUTO

service_DATA = kpresenter_ooimpress_import.desktop kpresenter_ooimpress_export.desktop

servicedir = $(kde_servicesdir)
