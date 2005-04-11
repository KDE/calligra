INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(LIBWPD_CFLAGS) $(all_includes)

kde_module_LTLIBRARIES = libwpimport.la 

libwpimport_la_SOURCES = wpimport.cc 
libwpimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libwpimport_la_LIBADD = $(KOFFICE_LIBS) $(LIBWPD_LIBS)

noinst_HEADERS = wpimport.h 

METASOURCES = AUTO

service_DATA = kword_wp_import.desktop 
servicedir = $(kde_servicesdir)
