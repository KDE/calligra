####### General stuff

INCLUDES= -I$(srcdir)/../../liboofilter $(KOFFICE_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = libooimpressimport.la


libooimpressimport_la_SOURCES = ooimpressimport.cc
libooimpressimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libooimpressimport_la_LIBADD  = ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)

METASOURCES = AUTO

service_DATA = kpresenter_ooimpress_import.desktop

servicedir = $(kde_servicesdir)
