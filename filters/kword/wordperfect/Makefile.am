####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libwpimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libwpimport_la_LIBADD = $(KOFFICE_LIBS)
libwpexport_la_LDFLAGS	= -module -avoid-version -no-undefined
libwpexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libwpimport.la libwpexport.la

libwpimport_la_SOURCES = wpimport.cc wpimport_factory.cc
libwpexport_la_SOURCES = wpexport.cc wpexport_factory.cc

noinst_HEADERS = wpimport.h wpimport_factory.h wpexport.h wpexport_factory.h

METASOURCES = AUTO

service_DATA = kword_wp_import.desktop kword_wp_export.desktop
servicedir = $(kde_servicesdir)
