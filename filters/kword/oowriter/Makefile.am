####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = liboowriterimport.la 


liboowriterimport_la_SOURCES = oowriterimport.cc stylestack.cc
liboowriterimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liboowriterimport_la_LIBADD  = $(KOFFICE_LIBS)

METASOURCES = AUTO

service_DATA = kword_oowriter_import.desktop

servicedir = $(kde_servicesdir)
