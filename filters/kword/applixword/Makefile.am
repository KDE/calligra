####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libapplixwordimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined 

####### Files

kde_module_LTLIBRARIES = libapplixwordimport.la

libapplixwordimport_la_SOURCES = applixwordimport.cc
libapplixwordimport_la_LIBADD = $(KOFFICE_LIBS)
noinst_HEADERS = applixwordimport.h

METASOURCES = AUTO

service_DATA = kword_applixword_import.desktop
servicedir = $(kde_servicesdir)
