####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libapplixspreadimport_la_LDFLAGS	= -module -avoid-version -no-undefined 

####### Files

lib_LTLIBRARIES = libapplixspreadimport.la

libapplixspreadimport_la_SOURCES = applixspreadimport.cc applixspreadimport_factory.cc
libapplixspreadimport_la_LIBADD = $(KOFFICE_LIBS)
noinst_HEADERS = applixspreadimport.h applixspreadimport_factory.h

METASOURCES = AUTO

service_DATA = kspread_applixspread_import.desktop
servicedir = $(kde_servicesdir)
