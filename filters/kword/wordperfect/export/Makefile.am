
INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../../libexport $(all_includes)
kde_module_LTLIBRARIES = libwpexport.la

libwpexport_la_SOURCES = wpexport.cc wp5.cc wp6.cc
libwpexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libwpexport_la_LIBADD = $(KOFFICE_LIBS) ../../libexport/libkwordexportfilters.la

noinst_HEADERS = wpexport.h wp5.h wp6.h

METASOURCES = AUTO

service_DATA = kword_wp_export.desktop
servicedir = $(kde_servicesdir)
