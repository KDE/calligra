####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libapplixspreadimport.la

libapplixspreadimport_la_SOURCES = applixspreadimport.cc
libapplixspreadimport_la_LIBADD = $(KOFFICE_LIBS)
libapplixspreadimport_la_LDFLAGS = -module $(KDE_PLUGIN)
noinst_HEADERS = applixspreadimport.h

METASOURCES = AUTO

service_DATA = kspread_applixspread_import.desktop
servicedir = $(kde_servicesdir)
