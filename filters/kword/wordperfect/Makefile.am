####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libwpimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libwpimport_la_LIBADD = $(KOFFICE_LIBS)
libwpexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libwpexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libwpimport.la libwpexport.la

libwpimport_la_SOURCES = wpimport.cc wpimport_factory.cc
libwpexport_la_SOURCES = wpexport.cc wpexport_factory.cc

noinst_HEADERS = wpimport.h wpimport_factory.h wpexport.h wpexport_factory.h

METASOURCES = AUTO

# better not install the .desktop file for the export filter as it's still empty
service_DATA = kword_wp_import.desktop # kword_wp_export.desktop
servicedir = $(kde_servicesdir)
